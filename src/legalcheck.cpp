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
#include "attacks.h"
#include "position.h"

static bool castle_path_is_clear(Position const &position, Square rook) {
    return !(position.get_bb() & CASTLE_OCC_MASK_BB[rook]);
}

bool Position::is_legal(Move move) const {
    if (move.data == 0)
        return false;

    auto from = move.from();
    auto to   = move.to();
    auto flag = move.flag();

    if (flag == MVEFLAG_NORMAL || flag == MVEFLAG_PROMOTION) {
        if (get_piece(from) == PCE_WKING || get_piece(from) == PCE_BKING) {
            auto occupancy = get_bb() ^ (1ull << from);
            return !square_is_attacked(*this, to, !side, occupancy);
        }

        else {
            auto occupancy = get_bb() ^ (1ull << from) ^ (1ull << to);
            auto enemy     = get_bb(!get_side());
            auto captured  = get_piece(to);

            if (captured != PCE_NULL) {
                occupancy ^= (1ull << to);
                enemy ^= (1ull << to);
            }

            auto pawns   = get_bb(PT_PAWN) & enemy;
            auto knights = get_bb(PT_KNIGHT) & enemy;
            auto bishops = get_bb(PT_BISHOP) & enemy;
            auto rooks   = get_bb(PT_ROOK) & enemy;
            auto queens  = get_bb(PT_QUEEN) & enemy;
            auto king    = get_lsb(get_bb(PT_KING, get_side()));

            bishops |= queens;
            rooks |= queens;
            return !((generate_pawn_attacks_bb(king, side) & pawns) || (generate_bishop_attacks_bb(king, occupancy) & bishops) || (generate_rook_attacks_bb(king, occupancy) & rooks) || (generate_knight_attacks_bb(king) & knights));
        }
    }

    else if (move.flag() == MVEFLAG_CASTLE) {
        return !square_is_attacked(*this, to, !side);
    }

    else {
        auto ep        = static_cast<Square>(to ^ 8);
        auto occupancy = get_bb() ^ (1ull << from) ^ (1ull << to) ^ (1ull << ep);
        auto enemy     = get_bb(!get_side()) ^ (1ull << ep);
        auto pawns     = get_bb(PT_PAWN) & enemy;
        auto knights   = get_bb(PT_KNIGHT) & enemy;
        auto bishops   = get_bb(PT_BISHOP) & enemy;
        auto rooks     = get_bb(PT_ROOK) & enemy;
        auto queens    = get_bb(PT_QUEEN) & enemy;
        auto king      = get_lsb(get_bb(PT_KING, get_side()));

        bishops |= queens;
        rooks |= queens;

        return !((generate_pawn_attacks_bb(king, side) & pawns) || (generate_bishop_attacks_bb(king, occupancy) & bishops) || (generate_rook_attacks_bb(king, occupancy) & rooks) || (generate_knight_attacks_bb(king) & knights));
    }

    return true;
}

bool Position::is_pseudolegal(Move move) const {
    if (!move.data)
        return false;

    auto from     = move.from();
    auto to       = move.to();
    auto flag     = move.flag();
    auto moving   = get_piece(from);
    auto captured = get_piece(to);

    if (moving == PCE_NULL)
        return false;

    if (compute_color(moving) != side)
        return false;

    if (captured != PCE_NULL && compute_color(captured) == side)
        return false;

    if (flag == MVEFLAG_CASTLE) {
        if (!test_bit(castle_rooks, to))
            return false;

        if (from != SQ_E1 && get_side() == CLR_WHITE)
            return false;

        if (from != SQ_E8 && get_side() == CLR_BLACK)
            return false;

        if (!castle_path_is_clear(*this, to))
            return false;

        if (square_is_attacked(*this, from, !get_side()))
            return false;

        auto path = CASTLE_CHECKS_MASK_BB[to];
        while (path) {
            Square sq = pop_lsb(path);
            if (square_is_attacked(*this, sq, !side))
                return false;
        }

        return true;
    }

    if (moving == PCE_WPAWN || moving == PCE_BPAWN) {
        auto start_rank = side == CLR_WHITE ? RANK_2 : RANK_7;
        auto forward    = side == CLR_WHITE ? DIR_NORTH : DIR_SOUTH;
        auto forward_sq = from + forward;
        auto prom_rank  = side == CLR_WHITE ? RANK_7_BB : RANK_2_BB;
        auto from_sq_bb = 1ull << from;

        if (flag == MVEFLAG_PROMOTION) {
            if ((prom_rank & from_sq_bb) == 0)
                return false;
        }

        if (from_sq_bb & prom_rank) {
            if (flag != MVEFLAG_PROMOTION)
                return false;
        }

        if (flag == MVEFLAG_NORMAL || flag == MVEFLAG_PROMOTION) {
            if (get_piece(to) == PCE_NULL) {
                auto double_push    = forward + forward;
                auto double_push_sq = from + double_push;

                if (to == forward_sq)
                    return true;

                return (to == double_push_sq && get_piece(static_cast<Square>(forward_sq)) == PCE_NULL && compute_rank(from) == start_rank);
            } else {
                return test_bit(generate_pawn_attacks_bb(from, side), to);
            }
        }

        else {
            if (to != ep_sq)
                return false;

            if (!((ADJ_FILES_BB[ep_sq]) & (1ull << from)))
                return false;

            captured = get_piece(static_cast<Square>(to ^ 8));

            if (captured == PCE_NULL || compute_color(captured) == side)
                return false;

            auto down_right = to - forward + DIR_EAST;
            auto down_left  = to - forward + DIR_WEST;

            return (from == down_left || from == down_right);
        }
    }

    else {
        if (flag != MVEFLAG_NORMAL)
            return false;

        auto attacks = generate_attacks_bb(compute_piece_type(moving), from, get_bb());
        return test_bit(attacks, to);
    }
    return false;
}