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
int lmp_margin[64][2]{0};
int quiet_lmp_margin[64][2]{0};

namespace
{
    constexpr int rfp_margin[6]
    {
        0, 120, 280, 510, 680, 850
    };

    constexpr int see_pruning_margins[5] 
    {
        0, -100, -200, -300, -325
    };

    enum
    {
        MinEval = -32001,
        MaxEval = -MinEval,
        MateEval = 32000,
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
                return Eval::evaluate(position);

            if (position.is_drawn())
                return 0;
        }

        int stand_pat = Eval::evaluate(position);

        if (stand_pat >= beta)
            return beta;

        alpha = std::max(alpha, stand_pat);

        MovePicker picker(search);

        for (Move move; picker.qnext(move);)
        {
            if (move.score + 300 < alpha)
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

    bool is_pv_node(int alpha, int beta)
    {
        return std::abs(alpha - beta) > 1;
    }

    void add_killer(Search::Info& search, Move move)
    {
        search.killers[search.stats.ply][1] = search.killers[search.stats.ply][0];
        search.killers[search.stats.ply][0] = move;
    }

    int16_t mate_score_to_tt(int score, int ply)
    {
        return score  >  MinMateScore ? score + ply : 
               score  < -MinMateScore ? score - ply : score;
    }

    int16_t tt_score_to_mate(int score, int ply)
    {
        return score  >  MinMateScore ? score - ply : 
               score  < -MinMateScore ? score + ply : score;
    }

    SearchResult pvs(Search::Info& search, int depth, int alpha, int beta, bool do_null = true)
    {
        if (search.limits.stopped)
            return 0;

        if (search.stats.ply)
            depth += search.position->king_in_check();

        if (depth <= 0)
            return qsearch(search, alpha, beta);

        search.update();

        SearchResult result;
        MovePicker   picker    = MovePicker(search);
        Position&    position  = *search.position;
        TEntry&      entry     = TT.retrieve(position);
        bool         pv_node   = is_pv_node(alpha, beta);
        bool         in_check  = position.king_in_check();
        bool         at_root   = search.stats.ply == 0;
        bool         tthit     = entry.hash == position.key.data();
        int          move_num  = 0;
        int          original  = alpha;

        if (!at_root)
        {
            if (search.stats.ply >= MaxPly)
                return Eval::evaluate(position);

            if (position.is_drawn())
                return 0;
        }

        if (entry.depth >= depth && tthit)
        {
            Move move = Move(entry.move);
            int score = tt_score_to_mate(entry.score, search.stats.ply);

            if (entry.flag == TEFlag::exact || 
               (entry.flag == TEFlag::lower && score >= beta) || 
               (entry.flag == TEFlag::upper && score <= alpha))
               {
                    if (!move_is_capture(position, move) && score >= beta)
                        history_bonus(search.history, position, move, depth);
                        
                    return { score, move };
               }
        }

        int eval = tthit ? entry.seval : Eval::evaluate(position);
        search.eval[search.stats.ply] = eval;

        bool improving = eval > search.eval[std::max(0, search.stats.ply - 2)];

        if (!at_root && !in_check && depth < 6 && (eval - rfp_margin[depth]) >= beta )
            return eval;

        if (!pv_node && !in_check && depth >= 4 && do_null && position.should_do_null() && eval + 300 >= beta)
        {
            int R = std::max(4, 3 + depth / 3);

            position.apply_null_move(search.stats.ply);
            int score = -pvs(search, depth - R, -beta, -beta + 1, false).score;
            position.revert_null_move(search.stats.ply);

            if (search.limits.stopped)
                return 0;

            if (score >= beta)
                return beta;
        }

        if (!tthit && depth > 3) 
            depth--;

        for (Move move; picker.next(move);)
        {
            if (move_num > lmp_margin[depth][improving])
                picker.skip_quiets = true;

            if (picker.stage >= MovePicker::Stage::GiveQuiet && move_num > quiet_lmp_margin[depth][improving])
                break;

            if (depth < 5 && move_is_capture(position, move) && move.score < see_pruning_margins[depth])
                continue;

            move_num++;

            position.apply_move(move, search.stats.ply);

            int score = 0;
            
            if (move_num > 3 && depth > 2)
            {
                int R = lmr_reductions_array[std::min(63, depth)][std::min(63, move_num)];
                int new_depth = depth - 1;

                R -= pv_node;
                R -= (picker.stage == MovePicker::Stage::Killer1 || picker.stage == MovePicker::Stage::Killer2);

                if (picker.stage == MovePicker::Stage::GiveQuiet)
                    R -= (get_history(search.history, position, move) / 14000);

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
                    update_history(search.history, position, move, picker.movelist, depth);
                    add_killer(search, move);
                }
                else 
                    update_history(search.capture_history, position, move, picker.movelist, depth);

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

        TEFlag flag = result.score <= original ? TEFlag::upper : result.score >= beta ? TEFlag::lower : TEFlag::exact;
        TT.add(position, result.best_move, mate_score_to_tt(result.score, search.stats.ply), depth, flag, eval);

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
        {
            o << "cp " << int(score * 100 / mg_score(PAWN_VALUE));
        }
        return o.str();
    }

    void print_info_string(SearchResult result, Search::Info &search, int depth)
    {
        using namespace std::chrono;
        std::cout << "info";
        std::cout << " depth " << depth;
        std::cout << " seldepth " << search.stats.seldepth;
        std::cout << " nodes " << search.stats.iter_nodes;
        std::cout << " score " << print_score(result.score);
        std::cout << " time " << search.limits.stopwatch.elapsed_time().count();
        std::cout << " pv ";

        for (auto m : TT.extract_pv(*search.position, depth))
        {
            std::cout << m << ' ';
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
                lmr_reductions_array[i][j] = log(i) * log(j);
            }

            lmp_margin[i][1] = 3 + 2 * i * i;
            lmp_margin[i][0] = 3 + i * i / 1.5;

            quiet_lmp_margin[i][0]  = 2 + i * i / 1.5;
            quiet_lmp_margin[i][1]  = 2 + i * i;
        }
    }

    uint64_t bestmove(Info& search, bool log)
    {
        Position& position = *search.position;
        if (PolyGlot::book.enabled)
        {
            Move bookmove = PolyGlot::book.probe(position);
            if (bookmove.data)
            {
                std::cout << "bestmove " << bookmove << std::endl;
                return 0;
            }
        }
        constexpr int window = 12;

        SEARCH_ABORT = false;
        Move best_move = NullMove;
        int score = 0;

        for (int depth = 1; depth <= search.limits.max_depth; depth++)
        {
            search.stats.reset_iteration();

            int alpha = MinEval;
            int beta  = MaxEval;
            int delta = window;

            if (depth > 5) 
            {
                int w = window - std::min(6, depth / 3);
                alpha = std::max(score - w, (int)MinEval);
                beta  = std::min(score + w, (int)MaxEval);
            }

            while(true)
            {
                auto result = pvs(search, depth, alpha, beta);
                if (search.limits.stopped) goto conc;

                best_move = result.best_move;
                score     = result.score;

                if (score <= alpha)
                {
                    beta  = (beta + alpha) / 2;
                    alpha = std::max(alpha - delta, (int)MinEval);
                }
                else if (score >= beta)
                    beta = std::min(beta + delta, (int)MaxEval);
                else 
                    break;

                delta *= 1.5;
            }

            if (log)
                print_info_string({ score, best_move }, search, depth);
        }
        conc:

        if (log)    
            std::cout << "bestmove " << best_move << std::endl;

        return search.stats.total_nodes;
    }

}