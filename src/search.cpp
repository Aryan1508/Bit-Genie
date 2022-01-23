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

namespace {
// Static-exchange-evaluation pruning
constexpr int SP_TABLE[5]{
    0, -100, -100, -300, -325
};

// Late-move pruning
int LMP_TABLE[65][2];

// Late-move reductions
int LMR_TABLE[65][64];

void apply_nullmove(SearchInfo &search) {
    search.position.apply_nullmove();
    search.ply++;
}

void revert_nullmove(SearchInfo &search) {
    search.position.revert_nullmove();
    search.ply--;
}

void apply_move(SearchInfo &search, Move move) {
    search.position.apply_move(move);
    search.ply++;
}

void revert_move(SearchInfo &search) {
    search.position.revert_move();
    search.ply--;
}

void update_info(SearchInfo &search) {
    search.nodes++;
    search.seldepth = std::max(search.ply, search.seldepth);

    if ((search.nodes & 2047) == 0)
        search.limits.update();
}

bool is_pv_node(int alpha, int beta) {
    return std::abs(alpha - beta) > 1;
}

int nmp_depth(int depth, int eval, int beta) {
    auto reduction = std::max(4, 3 + depth / 3) + std::clamp((eval - beta) / 256, 0, 2);
    return depth - reduction;
}

void update_tt_after_search(SearchInfo &search, SearchResult result, int depth, int original, int beta, int eval) {
    auto flag = result.score <= original ? TTFLAG_UPPER : result.score >= beta ? TTFLAG_LOWER
                                                                               : TTFLAG_EXACT;

    add_tt_entry(search, TEntry(search.position.get_key(), result.score, result.best_move, depth, flag, eval));
}

void update_history_tables_on_cutoff(SearchInfo &search, Movelist const &other, Move move, int depth) {
    auto &p = search.position;
    if (move_is_capture(p, move))
        update_history(search.capture_history, p, move, other, depth);
    else {
        update_history(search.history, p, move, other, depth);

        if (p.previous_move() != MOVE_NULL)
            update_history(search.counter_history, p, move, other, depth);
        add_killer(search.killers, search.ply, move);
    }
}

void update_search_result(SearchResult &result, int score, Move move) {
    if (score > result.score) {
        result.best_move = move;
        result.score     = score;
    }
}

int calculate_rfp_margin(int eval, int depth, bool improving) {
    auto margin = 100 * depth;
    margin /= (improving + 1);
    return eval - margin;
}

SearchResult pvs(SearchInfo &search, int depth, int alpha, int beta, bool do_null = true) {
    if (search.limits.stopped)
        return 0;

    if (search.ply)
        depth += search.position.king_in_check();

    if (depth <= 0)
        return qsearch(search, alpha, beta);

    update_info(search);

    SearchResult result;
    auto picker    = MovePicker(search);
    auto &position = search.position;
    auto &entry    = retrieve_tt_entry(search);
    auto pv_node   = is_pv_node(alpha, beta);
    auto in_check  = position.king_in_check();
    auto at_root   = search.ply == 0;
    auto tthit     = entry.hash == position.get_key();
    auto move_num  = 0;
    auto original  = alpha;

    if (!at_root) {
        if (search.ply >= MAX_PLY)
            return position.static_evaluation();

        if (position.drawn())
            return 0;
    }

    if (entry.depth >= depth && tthit && !pv_node) {
        Move move = Move(entry.move);

        if (entry.flag == TTFLAG_EXACT ||
            (entry.flag == TTFLAG_LOWER && entry.score >= beta) ||
            (entry.flag == TTFLAG_UPPER && entry.score <= alpha)) {
            if (entry.score >= beta)
                update_history_tables_on_cutoff(search, picker.movelist, move, depth);

            return { entry.score, move };
        }
    }

    auto eval               = tthit ? entry.seval : position.static_evaluation();
    search.eval[search.ply] = eval;
    auto improving          = eval > search.eval[std::max(0, search.ply - 2)];

    if (!pv_node && !in_check && calculate_rfp_margin(eval, depth, improving) >= beta)
        return eval;

    if (!pv_node && !in_check && depth == 1 && eval + 400 <= alpha)
        return qsearch(search, alpha, beta);

    const bool is_pawn_eg = !(position.get_bb() & ~(position.get_bb(PT_KING) | position.get_bb(PT_PAWN)));
    if (!pv_node && !in_check && depth >= 4 && do_null && (popcount64(position.get_bb()) > 5 && !is_pawn_eg) && eval + 300 >= beta) {
        apply_nullmove(search);
        int score = -pvs(search, nmp_depth(depth, eval, beta), -beta, -beta + 1, false).score;
        revert_nullmove(search);

        if (search.limits.stopped)
            return 0;

        if (score >= beta)
            return beta;
    }

    if (!tthit && depth > 3)
        depth--;

    for (Move move; picker.next(move);) {
        bool is_quiet = !move_is_capture(position, move);

        if (move_num > LMP_TABLE[depth][improving])
            break;

        if (depth < 5 && !is_quiet && move.score < SP_TABLE[depth])
            continue;

        move_num++;
        apply_move(search, move);

        auto score = 0;
        if (depth > 2 && move_num > 3) {
            int R         = LMR_TABLE[depth][std::min(63, move_num)];
            
            R -= pv_node;
            R -= move == picker.killer1 || move == picker.killer2;

            if (picker.stage == STAGE_QUIET)
                R -= get_history(search.history, search.position, move) / 14000;

            R -= picker.stage == STAGE_GOOD_NOISY;

            int new_depth = std::clamp(depth - 1 - R, 1, depth - 2);
            score = -pvs(search, new_depth, -alpha - 1, -alpha).score;

            if (score > alpha && new_depth < depth - 1)
                score = -pvs(search, depth - 1, -beta, -alpha).score;
        } else {
            if (move_num == 1)
                score = -pvs(search, depth - 1, -beta, -alpha).score;
            else {
                score = -pvs(search, depth - 1, -alpha - 1, -alpha).score;

                if (alpha < score && score < beta)
                    score = -pvs(search, depth - 1, -beta, -score).score;
            }
        }

        revert_move(search);

        if (search.limits.stopped)
            return 0;

        update_search_result(result, score, move);

        alpha = std::max(alpha, score);
        if (alpha >= beta) {
            update_history_tables_on_cutoff(search, picker.movelist, move, depth);
            break;
        }
    }

    if (move_num == 0) {
        if (in_check)
            return search.ply - MATE_EVAL;
        else
            return 0;
    }

    if (search.limits.stopped)
        return 0;

    update_tt_after_search(search, result, depth, original, beta, eval);
    return result;
}

int mate_distance(int score) {
    if (score > 0)
        return (MATE_EVAL - score) / 2 + 1;
    else
        return -((MATE_EVAL + score) / 2 + 1);
}

std::string print_score(int score) {
    std::stringstream o;
    if (score > MIN_MATE_EVAL || score < -MIN_MATE_EVAL) {
        o << "mate " << mate_distance(score);
    } else {
        o << "cp " << score;
    }
    return o.str();
}

void print_info_string(SearchResult result, SearchInfo &search, int depth) {
    std::cout << "info";
    std::cout << " depth " << depth;
    std::cout << " seldepth " << search.seldepth;
    std::cout << " nodes " << search.nodes;
    std::cout << " score " << print_score(result.score);
    std::cout << " time " << search.limits.stopwatch.elapsed_time().count();
    std::cout << " pv ";

    for (auto m : TT.extract_pv(search.position, depth)) {
        std::cout << m << ' ';
    }

    std::cout << std::endl;
}
}

void init_search_tables() {
    for (int i = 0; i < 65; i++) {
        for (int j = 0; j < 64; j++) {
            LMR_TABLE[i][j] = log(i) * log(j) / 1.2;
        }

        LMP_TABLE[i][0] = 2 + i * i / 1.5;
        LMP_TABLE[i][1] = 2 + i * i;
    }
}

SearchResult search_position(SearchInfo &search, bool log) {
    Position &position = search.position;
    if (PolyGlot::book.enabled) {
        Move bookmove = PolyGlot::book.probe(position);
        if (bookmove.data) {
            std::cout << "bestmove " << bookmove << std::endl;
        }
    }
    SearchResult result;
    constexpr int window = 12;
    SEARCH_ABORT         = false;
    auto score           = 0;
    auto best_move       = MOVE_NULL;

    for (auto depth = 1; depth <= search.limits.max_depth; depth++) {
#ifdef FEN_GENERATOR
    if (depth > FEN_GENERATOR_DEPTH || search.nodes > FEN_GENERATOR_NODES) {
        break;
    }
#endif
        search.ply = search.seldepth = 0;

        auto alpha = MIN_EVAL;
        auto beta  = MAX_EVAL;
        auto delta = window;

        if (depth > 5) {
            auto w = window - std::min(6, depth / 3);
            alpha  = std::max(score - w, MIN_EVAL);
            beta   = std::min(score + w, MAX_EVAL);
        }

        while (true) {
            result = pvs(search, depth, alpha, beta);

            if (search.limits.stopped)
                goto conc;

            score = result.score;
            if (score <= alpha) {
                beta  = (beta + alpha) / 2;
                alpha = std::max(alpha - delta, (int)MIN_EVAL);
            } else if (score >= beta) {
                beta      = std::min(beta + delta, (int)MAX_EVAL);
                best_move = result.best_move;
            } else
                break;
            delta *= 1.5;
        }
        best_move = result.best_move;

        if (log)
            print_info_string({ score, best_move }, search, depth);
    }
conc:
    if (log)
        std::cout << "bestmove " << best_move << std::endl;

    return { score, best_move };
}

int qsearch(SearchInfo &search, int alpha, int beta) {
    if (search.limits.stopped)
        return 0;

    update_info(search);
    auto &position = search.position;
    auto at_root   = search.ply == 0;

    if (!at_root) {
        if (search.ply >= MAX_PLY)
            return position.static_evaluation();

        if (position.drawn())
            return 0;
    }

    auto stand_pat = position.static_evaluation();
    if (stand_pat >= beta)
        return beta;
    alpha = std::max(alpha, stand_pat);

    MovePicker picker(search);
    for (Move move; picker.qnext(move);) {
        apply_move(search, move);
        auto score = -qsearch(search, -beta, -alpha);
        revert_move(search);

        if (search.limits.stopped)
            return 0;

        alpha = std::max(alpha, score);

        if (alpha >= beta)
            return beta;
    }

    return alpha;
}