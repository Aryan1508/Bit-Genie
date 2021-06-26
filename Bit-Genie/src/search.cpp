/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "search.h"
#include "movegen.h"
#include "position.h"
#include "eval.h"
#include "moveorder.h"
#include "polyglot.h"
#include "tt.h"
#include <sstream>
#include "evalscores.h"
#include <cmath>
#include <algorithm>

int lmr_reductions_array[64][64]{0};

std::atomic_bool SEARCH_ABORT = ATOMIC_VAR_INIT(false);

namespace
{
    enum
    {
        MinEval = -std::numeric_limits<int>::max(),
        MaxEval = -MinEval,
        MateEval = MaxEval - 1,
        MaxPly = 64,
        MinMateScore = MateEval - MaxPly,
    };

    struct SearchResult
    {
        int score = MinEval;
        Move best_move = NullMove;

        SearchResult() = default;

        SearchResult(int best_score, Move best = NullMove)
            : score(best_score), best_move(best)
        {
        }
    };

    int qsearch(Search::Info& search, int alpha, int beta)
    {
        if (search.limits.stopped)
            return 0;

        search.update();
        Position& position = *search.position;
        bool      at_root   = search.stats.ply == 0;

        if (!at_root)
        {
            if (search.stats.ply >= MaxPly)
                return eval_position(position);

            if (position.history.is_drawn(position.key) || position.half_moves >= 100)
                return 0;
        }

        int stand_pat = eval_position(position);

        if (stand_pat >= beta)
            return beta;

        alpha = std::max(alpha, stand_pat);

        MoveGenerator<true> gen;
        gen.generate<MoveGenType::noisy>(position);

        sort_qmovelist(gen.movelist, search);

        for (auto move : gen.movelist)
        {
            if (move_score(move) < 0)
                break;

            if (move_score(move) + 300 < alpha)
                break;

            position.apply_move(move, search.stats.ply);

            int score = -qsearch(search, -beta, -alpha);

            position.revert_move(search.stats.ply);

            if (search.limits.stopped)
                return 0;

            alpha = std::max(alpha, score);

            if (alpha >= beta)
                return beta;
        }

        return alpha;
    }

    SearchResult pvs(Search::Info& search, int depth, int alpha, int beta, bool do_null = true)
    {
        if (search.limits.stopped)
            return 0;

        if (depth <= 0)
            return qsearch(search, alpha, beta);

        Position& position = *search.position;
        bool      pv_node = std::abs(alpha - beta) > 1;
        bool      at_root = search.stats.ply == 0;
        
        search.update();

        if (!at_root)
        {
            if (search.stats.ply >= MaxPly)
                return eval_position(position);

            if (position.history.is_drawn(position.key) || position.half_moves >= 100)
                return 0;
        }

        TEntry& entry = TT.retrieve(position);
        bool tthit = entry.hash == position.key.data();

        if (entry.depth >= depth && tthit)
        {
            if (entry.flag == TEFlag::exact || 
               (entry.flag == TEFlag::lower && entry.score >= beta) || 
               (entry.flag == TEFlag::upper && entry.score <= alpha))
                return { entry.score, (Move)entry.move };
        }

        bool in_check = position.king_in_check();
        if (!pv_node && !in_check && depth > 4 && !at_root && do_null && position.should_do_null())
        {
            position.apply_null_move(search.stats.ply);
            int score = -pvs(search, depth - 4, -beta, -beta + 1, false).score;
            position.revert_null_move(search.stats.ply);

            if (search.limits.stopped)
                return 0;

            if (score >= beta)
                return beta;
        }

        SearchResult result;
        int move_num = 0;
        int original = alpha;
        int move_quiet_num = 0;

        MovePicker picker(search);
        
        if (!tthit && depth > 3) 
            depth--;

        for (Move move; picker.next(move);)
        {
            if (picker.stage >= MovePicker::Stage::GiveQuiet && move_quiet_num > int(picker.gen.movelist.size() / (3 - pv_node) + depth * 2))
                break;

            move_num++;
            if (picker.stage == MovePicker::Stage::GiveQuiet)
                move_quiet_num++;

            position.apply_move(move, search.stats.ply);

            int score = 0;

            if (move_num > 3 && depth > 2)
            {
                int R = lmr_reductions_array[std::min(63, depth)][std::min(63, move_num)];
                int new_depth = depth - 1;

                R -= pv_node;
                R -= (picker.stage == MovePicker::Stage::Killer1 || picker.stage == MovePicker::Stage::Killer2);

                if (picker.stage == MovePicker::Stage::GiveQuiet)
                    R -= (search.history.get(position, move) / 14000);

                int RDepth = std::clamp(new_depth - R, 1, new_depth - 1);
                score = -pvs(search, RDepth, -alpha - 1, -alpha).score;

                if (score > alpha)
                    score = -pvs(search, depth - 1, -beta, -alpha).score;
            }
            else
            {
                if (move_num == 1)
                    score = -pvs(search, depth - 1, -beta, -alpha).score;
                else
                {
                    score = -pvs(search, depth - 1, -alpha - 1, -alpha).score;

                    if (alpha < score && score < beta)
                        score = -pvs(search, depth - 1, -beta, -score).score;
                }
            }

            position.revert_move(search.stats.ply);

            if (search.limits.stopped)
                return 0;

            if (score > result.score)
            {
                result.best_move = move;
                result.score = score;
            }

            alpha = std::max(alpha, score);

            if (alpha >= beta)
            {
                if (!move_is_capture(position, move))
                {
                    search.history.add(position, move, depth * depth);
                    search.history.penalty(position, picker.gen.movelist, move, depth);
                    search.killers.add(search.stats.ply, move);
                }
                break;
            }
        }

        if (move_num == 0)
        {
            if (in_check)
                return search.stats.ply - MateEval;

            else
                return 0;
        }

        if (search.limits.stopped)
            return 0;

        TEFlag flag = result.score <= original ? TEFlag::upper : result.score >= beta ? TEFlag::lower : TEFlag::upper;
        TT.add(position, result.best_move, result.score, depth, flag);

        return result;
    }

    int mate_distance(int score)
    {
        if (score > 0)
            return (MateEval - score) / 2 + 1;
        else
            return -((MateEval + score) / 2 + 1);
    }

    std::string print_score(int score)
    {
        std::stringstream o;
        if (score > MinMateScore || score < -MinMateScore)
        {
            o << "mate " << mate_distance(score);
        }
        else
#define mg_score(s) ((int16_t)((uint16_t)((unsigned)((s)))))
        {
            o << "cp " << int(score * 100 / mg_score(PawnEval::value));
        }
        return o.str();
    }

    std::vector<Move> get_pv(Position position, int depth)
    {
        std::vector<Move> pv;

        TEntry *entry = &TT.retrieve(position);

        while (entry->hash == position.key.data() && depth != 0)
        {
            if (position.move_exists((Move)entry->move))
            {
                position.apply_move((Move)entry->move);
                pv.push_back((Move)entry->move);
                depth--;
            }
            else
                break;

            entry = &TT.retrieve(position);
        }

        return pv;
    }

    void print_info_string(Position &position, SearchResult &result, Search::Info &search, int depth)
    {
        using namespace std::chrono;
        std::cout << "info";
        std::cout << " depth " << depth;
        std::cout << " seldepth " << search.stats.seldepth;
        std::cout << " nodes " << search.stats.iter_nodes;
        std::cout << " score " << print_score(result.score);
        std::cout << " time " << search.limits.stopwatch.elapsed_time().count();
        std::cout << " pv ";

        for (auto m : get_pv(position, depth))
        {
            std::cout << print_move(m) << ' ';
        }

        std::cout << std::endl;
    }
}

namespace Search 
{
    void Info::update()
    {
        stats.update();
        
        if ((stats.iter_nodes & 2047) == 0)
            limits.update();
    }

    void init()
    {
        for (int i = 0; i < 64; i++)
        {
            for(int j = 0;j < 64;j++)
            {
                lmr_reductions_array[i][j] = log(i) * log(j) / 1.2;
            }
        }
    }

    void bestmove(Info search)
    {
        Position& position = *search.position;
        if (PolyGlot::book.enabled)
        {
            Move bookmove = PolyGlot::book.probe(position);
            if (bookmove)
            {
                std::cout << "bestmove " << print_move(bookmove) << std::endl;
                return;
            }
        }

        SEARCH_ABORT = false;

        Move best_move = NullMove;
        for (int depth = 1;
            depth <= search.limits.max_depth;
            depth++)
        {
            search.stats.reset_iteration();

            SearchResult result = pvs(search, depth, MinEval, MaxEval);

            if (search.limits.stopped)
                break;

            print_info_string(position, result, search, depth);
            best_move = result.best_move;
        }
        std::cout << "bestmove " << print_move(best_move) << std::endl;
    }

    uint64_t bench(Position &position)
    {
        SEARCH_ABORT = false;
        Info search;
        search.position = &position;

        for (int depth = 1;
            depth <= 12;
            depth++)
            pvs(search, depth, MinEval, MaxEval);

        return search.stats.total_nodes;
    }
}