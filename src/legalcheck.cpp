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
    return !(position.get_bb() & BitMask::castle_piece_path[rook]);
}

bool Position::is_legal(Move move) const {
    if (move.data == 0)
        return false;

    auto from = move.from();
    auto to   = move.to();
    auto flag = move.flag();

    if (flag == Move::Flag::normal || flag == Move::Flag::promotion) {
        if (get_piece(from) == PCE_WKING || get_piece(from) == PCE_BKING) {
            std::uint64_t occupancy = get_bb() ^ (1ull << from);
            return !Attacks::square_attacked(*this, to, !side, occupancy);
        }

        else {
            std::uint64_t occupancy = get_bb() ^ (1ull << from) ^ (1ull << to);
            std::uint64_t enemy     = get_bb(!get_side());

            Piece captured = get_piece(to);

            if (captured != PCE_NULL) {
                occupancy ^= (1ull << to);
                enemy ^= (1ull << to);
            }

            std::uint64_t pawns   = get_bb(PT_PAWN) & enemy;
            std::uint64_t knights = get_bb(PT_KNIGHT) & enemy;
            std::uint64_t bishops = get_bb(PT_BISHOP) & enemy;
            std::uint64_t rooks   = get_bb(PT_ROOK) & enemy;
            std::uint64_t queens  = get_bb(PT_QUEEN) & enemy;

            Square king = get_lsb(get_bb(PT_KING, get_side()));

            bishops |= queens;
            rooks |= queens;

            return !(
                (BitMask::pawn_attacks[side][king] & pawns) ||
                (Attacks::bishop(king, occupancy) & bishops) ||
                (Attacks::rook(king, occupancy) & rooks) ||
                (Attacks::knight(king) & knights));
        }
    }

    else if (move.flag() == Move::Flag::castle) {
        return !Attacks::square_attacked(*this, to, !side);
    }

    else {
        Square ep = static_cast<Square>(to ^ 8);
        std::uint64_t occupancy =
            get_bb() ^ (1ull << from) ^ (1ull << to) ^ (1ull << ep);
        std::uint64_t enemy = get_bb(!get_side()) ^ (1ull << ep);

        std::uint64_t pawns   = get_bb(PT_PAWN) & enemy;
        std::uint64_t knights = get_bb(PT_KNIGHT) & enemy;
        std::uint64_t bishops = get_bb(PT_BISHOP) & enemy;
        std::uint64_t rooks   = get_bb(PT_ROOK) & enemy;
        std::uint64_t queens  = get_bb(PT_QUEEN) & enemy;

        Square king = get_lsb(get_bb(PT_KING, get_side()));

        bishops |= queens;
        rooks |= queens;

        return !(
            (BitMask::pawn_attacks[side][king] & pawns) ||
            (Attacks::bishop(king, occupancy) & bishops) ||
            (Attacks::rook(king, occupancy) & rooks) ||
            (Attacks::knight(king) & knights));
    }

    return true;
}

bool Position::is_pseudolegal(Move move) const {
    if (!move.data)
        return false;

    Square from     = move.from();
    Square to       = move.to();
    Move::Flag flag = move.flag();
    Piece moving    = get_piece(from);
    Piece captured  = get_piece(to);

    if (moving == PCE_NULL)
        return false;

    if (compute_color(moving) != side)
        return false;

    if (captured != PCE_NULL && compute_color(captured) == side)
        return false;

    if (flag == Move::Flag::castle) {
        if (!test_bit(castle_rooks, to))
            return false;

        if (!castle_path_is_clear(*this, to))
            return false;

        if (Attacks::square_attacked(*this, from, !get_side()))
            return false;

        std::uint64_t path = BitMask::castle_attack_path[to];
        while (path) {
            Square sq = pop_lsb(path);
            if (Attacks::square_attacked(*this, sq, !side))
                return false;
        }

        return true;
    }

    if (moving == PCE_WPAWN || moving == PCE_BPAWN) {
        Rank start_rank   = side == CLR_WHITE ? RANK_2 : RANK_7;
        Direction forward = side == CLR_WHITE ? DIR_NORTH : DIR_SOUTH;
        auto forward_sq   = from + forward;
        std::uint64_t prom_rank =
            side == CLR_WHITE ? BitMask::rank7 : BitMask::rank2;
        std::uint64_t from_sq_bb = 1ull << from;

        if (flag == Move::Flag::promotion) {
            if ((prom_rank & from_sq_bb) == 0)
                return false;
        }

        if (from_sq_bb & prom_rank) {
            if (flag != Move::Flag::promotion)
                return false;
        }

        if (flag == Move::Flag::normal || flag == Move::Flag::promotion) {
            if (get_piece(to) == PCE_NULL) {
                const auto double_push    = forward + forward;
                const auto double_push_sq = from + double_push;

                if (to == forward_sq)
                    return true;

                return (
                    to == double_push_sq &&
                    get_piece(static_cast<Square>(forward_sq)) == PCE_NULL &&
                    compute_rank(from) == start_rank);
            } else {
                return test_bit(BitMask::pawn_attacks[side][from], to);
            }
        } else {
            if (to != ep_sq)
                return false;

            if (!((BitMask::neighbor_files[ep_sq]) & (1ull << from)))
                return false;

            captured = get_piece(static_cast<Square>(to ^ 8));

            if (captured == PCE_NULL || compute_color(captured) == side)
                return false;

            const auto down_right = to - forward + DIR_EAST;
            const auto down_left  = to - forward + DIR_WEST;

            return (from == down_left || from == down_right);
        }
    }

    else {
        if (flag != Move::Flag::normal)
            return false;

        std::uint64_t attacks =
            Attacks::generate(compute_piece_type(moving), from, get_bb());
        return test_bit(attacks, to);
    }
    return false;
}