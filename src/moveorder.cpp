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
#include "moveorder.h"
#include "attacks.h"
#include "tt.h"
#include "search.h"
#include <algorithm>

namespace {
uint64_t least_valuable_attacker(Position &position, uint64_t attackers, Color side, Piece &capturing) {
    for (int i = 0; i < 6; i++) {
        PieceType pt    = static_cast<PieceType>(i);
        capturing       = make_piece(pt, side);
        uint64_t pieces = position.get_bb(pt, side) & attackers;

        if (pieces)
            return pieces & -pieces;
    }
    return 0;
}

int16_t see(Position &position, Move move) {
    static constexpr int see_piece_vals[]{
        100, 300, 325, 500, 900, 1000,
        100, 300, 325, 500, 900, 1000, 0
    };

    int16_t scores[32] = { 0 };
    auto index         = 0;
    auto from          = move.from();
    auto to            = move.to();
    auto capturing     = position.get_piece(from);
    auto captured      = position.get_piece(to);
    auto attacker      = compute_color(capturing);
    auto attack_def    = generate_sq_attackers_bb(position, to);
    auto from_set      = (1ull << from);
    auto occ           = position.get_bb();
    auto bishops       = position.get_bb(PT_BISHOP);
    auto rooks         = position.get_bb(PT_ROOK);

    bishops |= position.get_bb(PT_QUEEN);
    rooks |= position.get_bb(PT_QUEEN);
    scores[index] = see_piece_vals[captured];
    do {
        index++;
        attacker      = !attacker;
        scores[index] = see_piece_vals[capturing] - scores[index - 1];

        if (-scores[index - 1] < 0 && scores[index] < 0)
            break;

        attack_def ^= from_set;
        occ ^= from_set;

        attack_def |= occ & ((generate_bishop_attacks_bb(to, occ) & bishops) | (generate_rook_attacks_bb(to, occ) & rooks));
        from_set = least_valuable_attacker(position, attack_def, attacker, capturing);
    } while (from_set);

    while (--index) {
        scores[index - 1] = -std::max<int16_t>(-scores[index - 1], scores[index]);
    }

    return scores[0];
}

template <bool quiet = false>
void score_movelist(Movelist &movelist, SearchInfo &search) {
    auto &position = search.position;
    for (auto &move : movelist) {
        if constexpr (!quiet) {
            int score = see(position, move);

            score += get_history(search.capture_history, position, move) / 128;

            if (move.flag() == MVEFLAG_PROMOTION)
                score += move.promoted() == PT_QUEEN ? 1000 : 300;

            move.score = score;
        } else {
            int score = get_history(search.history, position, move);

            if (move.flag() == MVEFLAG_PROMOTION)
                score += 10000;

            if (position.previous_move() != MOVE_NULL)
                score += get_history(search.counter_history, position, move);

            score = std::clamp(score, -32767, 32767);

            move.score = score;
        }
    }
}

void bubble_top_move(Movelist::iterator begin, Movelist::iterator end) {
    auto best = std::max_element(begin, end);
    std::iter_swap(best, begin);
}
}

MovePicker::MovePicker(SearchInfo &s)
    : search(&s) {
    stage = STAGE_HASH_MOVE;
}

bool MovePicker::qnext(Move &move) {
    auto &position = search->position;

    if (stage == STAGE_HASH_MOVE) {
        position.generate_noisy(movelist);

        score_movelist<false>(movelist, *search);
        bubble_top_move(movelist.begin(), movelist.end());
        current = movelist.begin();

        stage = STAGE_GOOD_NOISY;
    }

    if (stage == STAGE_GOOD_NOISY) {
        stage = STAGE_GEN_QUIET;
        if (current != movelist.end() && current->score >= 0) {
            move = *current++;
            bubble_top_move(current, movelist.end());
            return true;
        }
    }
    return false;
}

bool MovePicker::next(Move &move) {
    auto &position = search->position;

    auto can_move = [&](Move m) {
        return position.is_pseudolegal(m) && position.is_legal(m);
    };

    if (stage == STAGE_HASH_MOVE) {
        stage       = STAGE_GEN_NOISY;
        auto &entry = TT.retrieve(position);
        auto hmove  = Move(entry.move);

        if (entry.hash == position.get_key() && can_move(hmove)) {
            move      = hmove;
            hash_move = move;
            return true;
        }
    }

    if (stage == STAGE_GEN_NOISY) {
        position.generate_noisy(movelist);

        score_movelist(movelist, *search);
        current = movelist.begin();

        stage = STAGE_GOOD_NOISY;
    }

    if (stage == STAGE_GOOD_NOISY) {
        bubble_top_move(current, movelist.end());
        if (current != movelist.end() && current->score >= 0) {
            move = *current++;

            if (move == hash_move)
                return next(move);

            return true;
        }

        stage = STAGE_KILLER_1;
    }

    if (stage == STAGE_KILLER_1) {
        stage       = STAGE_KILLER_2;
        auto killer = search->killers[search->ply][0];

        if (can_move(killer) && !move_is_capture(position, killer)) {
            move    = killer;
            killer1 = move;
            return true;
        }
    }

    if (stage == STAGE_KILLER_2) {
        stage       = STAGE_BAD_NOISY;
        auto killer = search->killers[search->ply][1];

        if (can_move(killer) && !move_is_capture(position, killer)) {
            move    = killer;
            killer2 = move;
            return true;
        }
    }

    if (stage == STAGE_BAD_NOISY) {
        bubble_top_move(current, movelist.end());
        if (current != movelist.end()) {
            move = *current++;

            if (move == hash_move)
                return next(move);

            return true;
        }

        stage = STAGE_GEN_QUIET;
    }

    if (stage == STAGE_GEN_QUIET && !skip_quiets) {
        movelist.clear();
        position.generate_quiet(movelist);

        score_movelist<true>(movelist, *search);
        current = movelist.begin();
        stage   = STAGE_QUIET;
    }

    if (stage == STAGE_QUIET && !skip_quiets) {
        bubble_top_move(current, movelist.end());
        if (current != movelist.end()) {
            move = *current++;

            if (move == hash_move || move == killer1 || move == killer2)
                return next(move);

            return true;
        }
    }
    return false;
}