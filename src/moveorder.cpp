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

#include "search.h"
#include "tt.h"
#include <algorithm>

namespace {
std::uint64_t least_valuable_attacker(Position &position, const std::uint64_t attackers, Color side, Piece &capturing) {
    for (int i = 0; i < 6; i++) {
        const auto pt     = static_cast<PieceType>(i);
        capturing         = make_piece(pt, side);
        const auto pieces = position.get_bb(pt, side) & attackers;

        if (pieces)
            return isolate_lsb(pieces);
    }
    return 0;
}

Score see(Position &position, const Move move) {
    static constexpr Score see_piece_vals[]{ 100, 300, 325, 500, 900, 1000,
                                             100, 300, 325, 500, 900, 1000, 0 };

    Score scores[32]    = { 0 };
    const auto from     = move.get_from();
    const auto to       = move.get_to();
    const auto captured = position.get_piece(to);
    auto capturing      = position.get_piece(from);
    auto attacker       = compute_color(capturing);
    auto from_set       = make_bb(from);
    auto occ            = position.get_bb();
    auto bishops        = position.get_bb(PT_BISHOP);
    auto rooks          = position.get_bb(PT_ROOK);
    auto attack_def     = position.attackers_to_sq(to);
    auto index          = 0;

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

        attack_def |= occ & ((compute_bishop_attack_bb(to, occ) & bishops) |
                             (compute_rook_attack_bb(to, occ) & rooks));
        from_set = least_valuable_attacker(position, attack_def, attacker, capturing);
    } while (from_set);

    while (--index) {
        scores[index - 1] = -std::max<Score>(-scores[index - 1], scores[index]);
    }

    return scores[0];
}

void score_quiet(Move &move, SearchInfo &search) {
    Position &position = search.position;
    int score          = get_history(search.history, position, move);

    if (move.get_flag() == MVEFLAG_PROMOTION)
        score += 10000;

    if (position.previous_move() != MOVE_NULL)
        score += get_history(search.counter_history, position, move);

    score = std::clamp(score, -32767, 32767);

    move.set_score(score);
}

void score_capture(Move &move, SearchInfo &search) {
    Position &position = search.position;
    int score          = see(position, move);

    score += get_history(search.capture_history, position, move) / 128;

    if (move.get_flag() == MVEFLAG_PROMOTION)
        score += move.get_promoted() == PT_QUEEN ? 1000 : 300;

    move.set_score(score);
}

template <typename ScoreFunc>
void score_movelist(Movelist &movelist, SearchInfo &search, ScoreFunc f) {
    for (auto &move : movelist)
        f(move, search);
}

void bubble_top_move(Movelist::iterator begin, Movelist::iterator end) {
    auto best = std::max_element(begin, end, [](const Move lhs, const Move rhs) { return lhs.get_score() < rhs.get_score(); });
    std::iter_swap(best, begin);
}
} // namespace

MovePicker::MovePicker(SearchInfo &s)
    : search(&s) {
    stage = Stage::HashMove;
}

bool MovePicker::qnext(Move &move) {
    Position &position = search->position;

    if (stage == Stage::HashMove) {
        position.generate_noisy(movelist);

        score_movelist(movelist, *search, score_capture);
        bubble_top_move(movelist.begin(), movelist.end());
        current = movelist.begin();

        stage = Stage::GiveGoodNoisy;
    }

    if (stage == Stage::GiveGoodNoisy) {
        stage = Stage::GenQuiet;
        if (current != movelist.end() && current->get_score() >= 0) {
            move = *current++;
            bubble_top_move(current, movelist.end());
            return true;
        }
    }
    return false;
}

bool MovePicker::next(Move &move) {
    Position &position = search->position;

    auto can_move = [&](Move m) {
        return position.is_pseudolegal(m) && position.is_legal(m);
    };

    if (stage == Stage::HashMove) {
        stage       = Stage::GenNoisy;
        auto &entry = TT.probe(position.get_hash());

        Move hmove = Move(entry.move);

        if (entry.hash == position.get_hash() && can_move(hmove)) {
            move      = hmove;
            hash_move = move;
            return true;
        }
    }

    if (stage == Stage::GenNoisy) {
        position.generate_noisy(movelist);

        score_movelist(movelist, *search, score_capture);
        current = movelist.begin();

        stage = Stage::GiveGoodNoisy;
    }

    if (stage == Stage::GiveGoodNoisy) {
        bubble_top_move(current, movelist.end());
        if (current != movelist.end() && current->get_score() >= 0) {
            move = *current++;

            if (move == hash_move)
                return next(move);

            return true;
        }

        stage = Stage::Killer1;
    }

    if (stage == Stage::Killer1) {
        stage       = Stage::Killer2;
        Move killer = search->killers[search->stats.ply][0];

        if (can_move(killer) && !move_is_capture(position, killer)) {
            move    = killer;
            killer1 = move;
            return true;
        }
    }

    if (stage == Stage::Killer2) {
        stage       = Stage::GiveBadNoisy;
        Move killer = search->killers[search->stats.ply][1];

        if (can_move(killer) && !move_is_capture(position, killer)) {
            move    = killer;
            killer2 = move;
            return true;
        }
    }

    if (stage == Stage::GiveBadNoisy) {
        bubble_top_move(current, movelist.end());
        if (current != movelist.end()) {
            move = *current++;

            if (move == hash_move)
                return next(move);

            return true;
        }

        stage = Stage::GenQuiet;
    }

    if (stage == Stage::GenQuiet && !skip_quiets) {
        movelist.clear();
        position.generate_quiet(movelist);

        score_movelist(movelist, *search, score_quiet);
        current = movelist.begin();
        stage   = Stage::GiveQuiet;
    }

    if (stage == Stage::GiveQuiet && !skip_quiets) {
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