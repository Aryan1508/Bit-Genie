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
#include "moveorder.h"
#include "position.h"

#include <cmath>
#include <sstream>
#include <algorithm>

namespace {

// Late-move reductions
Depth LMR_TABLE[65][64]{ 0 };

// Late-move pruning
std::int16_t LMP_TABLE[65][2];

// Static-exchange evaluation pruning
constexpr Score SP_TABLE[5]{ 0, -100, -100, -300, -325 };

void update_search_info(SearchInfo &search) {
    search.stats.nodes++;
    search.stats.seldepth = std::max(search.stats.seldepth, search.stats.ply);

    if (search.stats.nodes % 2046 == 0) {
        const bool overtime   = search.limits.stopwatch.elapsed_time().count() >= search.limits.movetime;
        search.limits.stopped = SEARCH_ABORT || (search.limits.time_set && overtime);
    }
}

void apply_nullmove(SearchInfo &search) {
    search.position.apply_nullmove();
    search.stats.ply++;
}

void revert_nullmove(SearchInfo &search) {
    search.position.revert_nullmove();
    search.stats.ply--;
}

void apply_move(SearchInfo &search, const Move move) {
    search.position.apply_move(move);
    search.stats.ply++;
}

void revert_move(SearchInfo &search) {
    search.position.revert_move();
    search.stats.ply--;
}

auto is_pv_node(const Score alpha, const Score beta) {
    return std::abs(alpha - beta) > 1;
}

void add_killer(SearchInfo &search, const Move move) {
    search.killers[search.stats.ply][1] = search.killers[search.stats.ply][0];
    search.killers[search.stats.ply][0] = move;
}

Score mate_score_to_tt(const Score score, const Depth ply) {
    return score > MIN_MATE_EVAL    ? score + ply
           : score < -MIN_MATE_EVAL ? score - ply
                                    : score;
}

Score tt_score_to_mate(const Score score, const Depth ply) {
    return score > MIN_MATE_EVAL    ? score - ply
           : score < -MIN_MATE_EVAL ? score + ply
                                    : score;
}

Depth nmp_depth(const Depth depth, const Score eval, const Score beta) {
    const auto reduction = std::max(4, 3 + depth / 3) + std::clamp((eval - beta) / 256, 0, 2);
    return depth - reduction;
}

void update_tt_after_search(SearchInfo &search, SearchResult result, const Depth depth, const Score original, const Score beta, const Score eval) {
    const auto flag  = result.score <= original ? TTFLAG_UPPER
                       : result.score >= beta   ? TTFLAG_LOWER
                                                : TTFLAG_EXACT;
    const auto hash  = search.position.get_hash();
    const auto move  = result.best_move;
    const auto score = mate_score_to_tt(result.score, search.stats.ply);
    const auto entry = TEntry{ hash, score, eval, move.get_move_bits(), depth, flag };
    store_tentry(entry);
}

void update_history_tables_on_cutoff(SearchInfo &search, Movelist const &other, const Move move, const Depth depth) {
    Position &position = search.position;

    if (move_is_capture(position, move))
        update_history(search.capture_history, position, move, other, depth);
    else {
        update_history(search.history, position, move, other, depth);

        if (position.previous_move() != MOVE_NULL)
            update_history(search.counter_history, position, move, other, depth);
        add_killer(search, move);
    }
}

bool use_lmr(const Depth depth, const std::uint8_t move_i) {
    return depth > 2 && move_i > 3;
}

void update_search_result(SearchResult &result, const Score score, const Move move) {
    if (score > result.score) {
        result.best_move = move;
        result.score     = score;
    }
}

Depth calculate_lmr_depth(SearchInfo &search, MovePicker &picker, Move move, const Depth depth, const std::uint8_t move_i, const bool pv_node) {
    const auto new_depth = depth - 1;
    auto reduction       = LMR_TABLE[depth][std::min<Depth>(63, move_i)];

    reduction -= pv_node;
    reduction -= picker.stage == MovePicker::Stage::Killer2;

    if (picker.stage == MovePicker::Stage::GiveQuiet)
        reduction -= (get_history(search.history, search.position, move) / 14000);

    reduction -= (picker.stage == MovePicker::Stage::GiveGoodNoisy);

    return std::clamp(new_depth - reduction, 1, new_depth - 1);
}

Score calculate_rfp_margin(const Score eval, const Depth depth, const bool improving) {
    auto margin = 100 * depth;
    margin /= (improving + 1);
    return eval - margin;
}

Score qsearch(SearchInfo &search, Score alpha, Score beta) {
    if (search.limits.stopped)
        return 0;

    update_search_info(search);
    auto &position     = search.position;
    const auto at_root = search.stats.ply == 0;

    if (!at_root) {
        if (search.stats.ply >= MAX_PLY)
            return position.static_evaluation();

        if (position.drawn())
            return 0;
    }

    const auto stand_pat = position.static_evaluation();

    if (stand_pat >= beta)
        return beta;

    alpha = std::max(alpha, stand_pat);

    MovePicker picker(search);

    for (Move move; picker.qnext(move);) {
        apply_move(search, move);
        const Score score = -qsearch(search, -beta, -alpha);
        revert_move(search);

        if (search.limits.stopped)
            return 0;

        alpha = std::max(alpha, score);

        if (alpha >= beta)
            return beta;
    }

    return alpha;
}

SearchResult pvs(SearchInfo &search, Depth depth, Score alpha, Score beta, const bool do_null = true) {
    if (search.limits.stopped)
        return 0;

    if (search.stats.ply)
        depth += search.position.king_in_check();

    if (depth <= 0)
        return qsearch(search, alpha, beta);

    update_search_info(search);

    SearchResult result;
    MovePicker picker  = MovePicker(search);
    Position &position = search.position;
    TEntry &entry      = TT.probe(position.get_hash());
    bool pv_node       = is_pv_node(alpha, beta);
    bool in_check      = position.king_in_check();
    bool at_root       = search.stats.ply == 0;
    bool tthit         = entry.hash == position.get_hash();
    int move_num       = 0;
    int original       = alpha;

    if (!at_root) {
        if (search.stats.ply >= MAX_PLY)
            return position.static_evaluation();

        if (position.drawn())
            return 0;
    }

    if (entry.depth >= depth && tthit) {
        const auto move  = Move(entry.move);
        const auto score = tt_score_to_mate(entry.score, search.stats.ply);

        if (entry.flag == TTFLAG_EXACT ||
            (entry.flag == TTFLAG_LOWER && score >= beta) ||
            (entry.flag == TTFLAG_UPPER && score <= alpha)) {
            if (score >= beta)
                update_history_tables_on_cutoff(
                    search, picker.movelist, move, depth);

            return { score, move };
        }
    }

    const auto eval               = tthit ? entry.seval : position.static_evaluation();
    search.eval[search.stats.ply] = eval;

    bool improving = eval > search.eval[std::max(0, search.stats.ply - 2)];

    if (!pv_node && !in_check &&
        calculate_rfp_margin(eval, depth, improving) >= beta)
        return eval;

    if (!pv_node && !in_check && depth == 1 && eval + 400 <= alpha)
        return qsearch(search, alpha, beta);

    if (!pv_node && !in_check && depth >= 4 && do_null &&
        (popcount64(position.get_bb()) >= 4) && eval + 300 >= beta) {
        apply_nullmove(search);
        int score =
            -pvs(search, nmp_depth(depth, eval, beta), -beta, -beta + 1, false)
                 .score;
        revert_nullmove(search);

        if (search.limits.stopped)
            return 0;

        if (score >= beta)
            return beta;
    }

    if (!tthit && depth > 3)
        depth--;

    for (Move move; picker.next(move);) {
        if (move_num > LMP_TABLE[depth][improving])
            break;

        if (depth < 5 && move_is_capture(position, move) &&
            move.get_score() < SP_TABLE[depth])
            continue;

        move_num++;
        apply_move(search, move);

        Score score = 0;

        if (use_lmr(depth, move_num)) {
            int new_depth = calculate_lmr_depth(
                search, picker, move, depth, move_num, pv_node);

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
            update_history_tables_on_cutoff(
                search, picker.movelist, move, depth);
            break;
        }
    }

    if (move_num == 0) {
        if (in_check)
            return search.stats.ply - MATE_EVAL;
        else
            return 0;
    }

    if (search.limits.stopped)
        return 0;

    update_tt_after_search(search, result, depth, original, beta, eval);
    return result;
}

Score mate_distance(const Score score) {
    if (score > 0)
        return (MATE_EVAL - score) / 2 + 1;
    else
        return -((MATE_EVAL + score) / 2 + 1);
}

std::string print_score(const Score score) {
    std::stringstream o;
    if (score > MIN_MATE_EVAL || score < -MIN_MATE_EVAL) {
        o << "mate " << mate_distance(score);
    } else {
        o << "cp " << score;
    }
    return o.str();
}

std::vector<Move> extract_pv(Position &position, int depth) {
    auto move_exists = [&](const Move move) {
        Movelist movelist;
        position.generate_legal(movelist);
        return std::find(movelist.begin(), movelist.end(), move) != movelist.end();
    };

    std::vector<Move> pv;
    auto entry    = &TT.probe(position.get_hash());
    auto distance = 0;

    while (entry->hash == position.get_hash() && depth != 0) {
        const auto pv_move = Move(entry->move);
        if (move_exists(pv_move)) {
            distance++;
            depth--;
            position.apply_move(pv_move);
            pv.push_back(pv_move);
        } else {
            break;
        }

        entry = &TT.probe(position.get_hash());
    }

    while (distance--)
        position.revert_move();

    return pv;
}

void print_info_string(SearchResult result, SearchInfo &search, int depth) {
    std::cout << "info";
    std::cout << " depth " << depth;
    std::cout << " seldepth " << static_cast<int>(search.stats.seldepth);
    std::cout << " nodes " << search.stats.nodes;
    std::cout << " score " << print_score(result.score);
    std::cout << " time " << search.limits.stopwatch.elapsed_time().count();
    std::cout << " pv ";

    for (auto m : extract_pv(search.position, depth)) {
        std::cout << m << ' ';
    }

    std::cout << std::endl;
}
} // namespace

void init_search_tables() {
    for (int i = 0; i < 65; i++) {
        for (int j = 0; j < 64; j++) {
            LMR_TABLE[i][j] = log(i) * log(j) / 1.2;
        }

        LMP_TABLE[i][0] = 2 + i * i / 1.5;
        LMP_TABLE[i][1] = 2 + i * i;
    }
}

SearchResult bestmove(SearchInfo &search, bool log) {
    constexpr std::int16_t window = 12;

    SearchResult result;
    Move best_move     = MOVE_NULL;
    std::int16_t score = 0;
    SEARCH_ABORT       = false;

    for (std::int16_t depth = 1; depth <= search.limits.max_depth; depth++) {
        search.stats.ply = search.stats.seldepth = 0;

        std::int16_t alpha = MIN_EVAL;
        std::int16_t beta  = MAX_EVAL;
        std::int16_t delta = window;

        if (depth > 5) {
            const std::int16_t w = window - std::min<std::int16_t>(6, depth / 3);
            alpha                = std::max<std::int16_t>(score - w, MIN_EVAL);
            beta                 = std::min<std::int16_t>(score + w, MAX_EVAL);
        }

        while (true) {
            result = pvs(search, depth, alpha, beta);
            if (search.limits.stopped)
                goto conc;

            score = result.score;

            if (score <= alpha) {
                beta  = (beta + alpha) / 2;
                alpha = std::max<std::int16_t>(alpha - delta, MIN_EVAL);
            } else if (score >= beta) {
                beta      = std::min<std::int16_t>(beta + delta, MAX_EVAL);
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

    result = { score, best_move };

    return result;
}