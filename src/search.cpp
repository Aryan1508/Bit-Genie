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
#include "tt.h"
#include "search.h"
#include "polyglot.h"
#include "position.h"
#include "moveorder.h"

#include <cmath>
#include <sstream>
#include <algorithm>

int lmr_reductions_array[65][64]{0};
int lmp_margin[65][2]{0};

namespace
{
    constexpr int see_pruning_margins[5] 
    {
        0, -100, -100, -300, -325
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

    void apply_nullmove(Search::Info& search)
    {
        search.position->apply_nullmove();
        search.stats.ply++;
    }

    void revert_nullmove(Search::Info& search)
    {
        search.position->revert_nullmove();
        search.stats.ply--;
    }

    void apply_move(Search::Info& search, Move move)
    {
        search.position->apply_move(move);
        search.stats.ply++;
    }

    void revert_move(Search::Info& search)
    {
        search.position->revert_move();
        search.stats.ply--;
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

    int nmp_depth(int depth, int eval, int beta)
    {
        int reduction = std::max(4, 3 + depth / 3) + std::clamp((eval - beta) / 256, 0, 2);
        return depth - reduction;
    }

    void update_tt_after_search(Search::Info& search, SearchResult result, int depth, int original, int beta, int eval)
    {
        TEFlag flag = result.score <= original ? TEFlag::upper : result.score >= beta ? TEFlag::lower : TEFlag::exact;
        search.tt.add(*search.position, result.best_move, mate_score_to_tt(result.score, search.stats.ply), depth, flag, eval);
    }

    void update_history_tables_on_cutoff(Search::Info& search, Movelist const& other, Move move, int depth)
    {
        Position& p = *search.position;

        if (move_is_capture(p, move))
            update_history(search.capture_history, p, move, other, depth);
        else 
        {
            update_history(search.history, p, move, other, depth);

            if (p.previous_move() != NullMove)
                update_history(search.counter_history, p, move, other, depth);
            add_killer(search, move);
        }
    }

    bool use_lmr(int depth, int move_i)
    {
        return depth > 2 && move_i > 3;
    }

    void update_search_result(SearchResult& result, int score, Move move)
    {
        if (score > result.score)
        {
            result.best_move = move;
            result.score = score;
        }
    }

    int calculate_lmr_depth(Search::Info& search, MovePicker& picker, Move move, int depth, int move_i, bool pv_node)
    {
        int R = lmr_reductions_array[depth][std::min(63, move_i)];
        int new_depth = depth - 1;

        R -= pv_node;
        R -= picker.stage == MovePicker::Stage::Killer2;

        if (picker.stage == MovePicker::Stage::GiveQuiet)
            R -= (get_history(search.history, *search.position, move) / 14000);

        R -= (picker.stage == MovePicker::Stage::GiveGoodNoisy);

        return std::clamp(new_depth - R, 1, new_depth - 1);
    }

    int calculate_rfp_margin(int eval, int depth, bool improving)
    {
        int margin = 100 * depth;

        margin /= (improving + 1);

        return eval - margin;
    }

    SearchResult pvs(Search::Info& search, int depth, int alpha, int beta, bool do_null = true)
    {
        if (search.stats.ply)
            depth += search.position->king_in_check();

        if (depth <= 0)
            return qsearch(search, alpha, beta);

        SearchResult result;
        MovePicker   picker    = MovePicker(search);
        Position&    position  = *search.position;
        TEntry&      entry     = search.tt.retrieve(position);
        bool         pv_node   = is_pv_node(alpha, beta);
        bool         in_check  = position.king_in_check();
        bool         at_root   = search.stats.ply == 0;
        bool         tthit     = entry.hash == position.get_key();
        int          move_num  = 0;
        int          original  = alpha;

        if (!at_root)
        {
            if (search.stats.ply >= MaxPly)
                return position.static_evaluation();

            if (position.drawn())
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
                    if (score >= beta)
                        update_history_tables_on_cutoff(search, picker.movelist, move, depth);
                        
                    return { score, move };
               }
        }

        int eval = tthit ? entry.seval : position.static_evaluation();
        search.eval[search.stats.ply] = eval;

        bool improving = eval > search.eval[std::max(0, search.stats.ply - 2)];

        if (!pv_node && !in_check && calculate_rfp_margin(eval, depth, improving) >= beta )
            return eval;

        if (!pv_node && !in_check && depth == 1 && eval + 400 <= alpha)
            return qsearch(search, alpha, beta);

        if (!pv_node && !in_check && depth >= 4 && do_null && (popcount64(position.get_bb()) >= 4) && eval + 300 >= beta)
        {
            apply_nullmove(search);
            int score = -pvs(search, nmp_depth(depth, eval, beta), -beta, -beta + 1, false).score;
            revert_nullmove(search);

            if (score >= beta)
                return beta;
        }

        if (!tthit && depth > 3) 
            depth--;

        for (Move move; picker.next(move);)
        {
            if (move_num > lmp_margin[depth][improving])
                break;

            if (depth < 5 && move_is_capture(position, move) && move.score < see_pruning_margins[depth])
                continue;

            move_num++;
            apply_move(search, move);

            int score = 0;
            
            if (use_lmr(depth, move_num))
            {
                int new_depth = calculate_lmr_depth(search, picker, move, depth, move_num, pv_node);

                score = -pvs(search, new_depth, -alpha - 1, -alpha).score;

                if (score > alpha && new_depth < depth - 1)
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

            revert_move(search);

            update_search_result(result, score, move);

            alpha = std::max(alpha, score);
            if (alpha >= beta)
            {
                update_history_tables_on_cutoff(search, picker.movelist, move, depth);
                break;
            }
        }

        if (move_num == 0)
        {
            if (in_check) return search.stats.ply - MateEval;
            else return 0;
        }

        update_tt_after_search(search, result, depth, original, beta, eval);
        return result;
    }
}

namespace Search 
{
    void init()
    {
        for (int i = 0; i < 65; i++)
        {
            for(int j = 0;j < 64;j++)
            {
                lmr_reductions_array[i][j] = log(i) * log(j) / 1.2;
            }

            lmp_margin[i][0]  = 2 + i * i / 1.5;
            lmp_margin[i][1]  = 2 + i * i;
        }
    }

    int qsearch(Search::Info& search, int alpha, int beta)
    {

        Position& position = *search.position;
        bool      at_root   = search.stats.ply == 0;

        if (!at_root)
        {
            if (search.stats.ply >= MaxPly)
                return position.static_evaluation();

            if (position.drawn())
                return 0;
        }

        int stand_pat = position.static_evaluation();

        if (stand_pat >= beta)
            return beta;

        alpha = std::max(alpha, stand_pat);

        MovePicker picker(search);

        for (Move move; picker.qnext(move);)
        {
            apply_move(search, move);
            int score = -qsearch(search, -beta, -alpha);
            revert_move(search);

            alpha = std::max(alpha, score);

            if (alpha >= beta)
                return beta;
        }

        return alpha;
    }

    std::pair<Move, int> bestmove(Info& search, bool)
    {
        constexpr int window = 12;

        SEARCH_ABORT = false;
        Move best_move = NullMove;
        SearchResult result;
        int score = 0;

        for (int depth = 1; depth <= search.limits.max_depth; depth++)
        {
            search.stats.ply=0;

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
                result = pvs(search, depth, alpha, beta);

                score  = result.score;

                if (score <= alpha)
                {
                    beta  = (beta + alpha) / 2;
                    alpha = std::max(alpha - delta, (int)MinEval);
                }
                else if (score >= beta)
                {
                    beta = std::min(beta + delta, (int)MaxEval);
                    best_move = result.best_move;
                }
                else 
                    break;

                delta *= 1.5;
            }
            best_move = result.best_move;
        }
        return { best_move, score };
    }

}