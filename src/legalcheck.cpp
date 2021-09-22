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

#include "position.h"

constexpr uint64_t ADJ_FILES_BB[64]{
    0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828,
    0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040, 0X00202020202020202, 0X00505050505050505,
    0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0,
    0X04040404040404040, 0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414,
    0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040, 0X00202020202020202,
    0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828, 0X05050505050505050,
    0X0a0a0a0a0a0a0a0a0, 0X04040404040404040, 0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a,
    0X01414141414141414, 0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040,
    0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828,
    0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040, 0X00202020202020202, 0X00505050505050505,
    0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0,
    0X04040404040404040, 0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414,
    0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040
};

constexpr uint64_t CASTLE_CHECK_MASK[64]{
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000008, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000020, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000800000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00002000000000000000, 0x0000000000000000000
};

bool Position::is_legal(Move move) const {
    if (move == MOVE_NULL)
        return false;

    const auto from = move.get_from();
    const auto to   = move.get_to();
    const auto flag = move.get_flag();

    if (flag == MVEFLAG_NORMAL || flag == MVEFLAG_PROMOTION) {
        if (get_piece(from) == PCE_WKING || get_piece(from) == PCE_BKING) {
            return !square_is_attacked(to, !side, get_bb() ^ make_bb(from));
        } else {
            auto occupancy = get_bb() ^ make_bb(from) ^ make_bb(to);
            auto enemy     = get_bb(!get_side());
            auto captured  = get_piece(to);

            if (captured != PCE_NULL) {
                occupancy ^= make_bb(to);
                enemy ^= make_bb(to);
            }

            const auto queens  = get_bb(PT_QUEEN) & enemy;
            const auto pawns   = get_bb(PT_PAWN) & enemy;
            const auto knights = get_bb(PT_KNIGHT) & enemy;
            const auto bishops = (get_bb(PT_BISHOP) | queens) & enemy;
            const auto rooks   = (get_bb(PT_ROOK) | queens) & enemy;
            const auto king    = get_lsb(get_bb(PT_KING, get_side()));

            return !(
                (compute_pawn_attack_bb(king, side) & pawns) ||
                (compute_bishop_attack_bb(king, occupancy) & bishops) ||
                (compute_rook_attack_bb(king, occupancy) & rooks) ||
                (compute_knight_attack_bb(king) & knights));
        }
    } else if (move.get_flag() == MVEFLAG_CASTLE) {
        return !square_is_attacked(to, !side);
    } else {
        const auto ep_bb = make_bb(static_cast<Square>(to ^ 8));
        auto occupancy   = get_bb() ^ make_bb(from) ^ make_bb(to) ^ ep_bb;
        auto enemy       = get_bb(!get_side()) ^ ep_bb;

        const auto queens  = get_bb(PT_QUEEN) & enemy;
        const auto pawns   = get_bb(PT_PAWN) & enemy;
        const auto knights = get_bb(PT_KNIGHT) & enemy;
        const auto bishops = (get_bb(PT_BISHOP) | queens) & enemy;
        const auto rooks   = (get_bb(PT_ROOK) | queens) & enemy;
        const auto king    = get_lsb(get_bb(PT_KING, get_side()));

        return !(
            (compute_pawn_attack_bb(king, side) & pawns) ||
            (compute_bishop_attack_bb(king, occupancy) & bishops) ||
            (compute_rook_attack_bb(king, occupancy) & rooks) ||
            (compute_knight_attack_bb(king) & knights));
    }

    return true;
}

bool Position::is_pseudolegal(Move move) const {
    if (move == MOVE_NULL)
        return false;

    Square from    = move.get_from();
    Square to      = move.get_to();
    MoveFlag flag  = move.get_flag();
    Piece moving   = get_piece(from);
    Piece captured = get_piece(to);

    if (moving == PCE_NULL)
        return false;

    if (compute_color(moving) != side)
        return false;

    if (captured != PCE_NULL && compute_color(captured) == side)
        return false;

    if (flag == MVEFLAG_CASTLE) {
        if (!test_bit(castle_rooks, to))
            return false;

        if (get_bb() & get_castling_path(to))
            return false;

        if (square_is_attacked(from, !get_side()))
            return false;

        std::uint64_t path = CASTLE_CHECK_MASK[to];
        while (path) {
            Square sq = pop_lsb(path);
            if (square_is_attacked(sq, !side))
                return false;
        }

        return true;
    }

    if (moving == PCE_WPAWN || moving == PCE_BPAWN) {
        Rank start_rank   = side == CLR_WHITE ? RANK_2 : RANK_7;
        Direction forward = side == CLR_WHITE ? DIR_NORTH : DIR_SOUTH;
        auto forward_sq   = from + forward;
        std::uint64_t prom_rank =
            side == CLR_WHITE ? RANK_7_BB : RANK_2_BB;
        std::uint64_t from_sq_bb = 1ull << from;

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
                const auto double_push    = forward + forward;
                const auto double_push_sq = from + double_push;

                if (to == forward_sq)
                    return true;

                return (
                    to == double_push_sq &&
                    get_piece(static_cast<Square>(forward_sq)) == PCE_NULL &&
                    compute_rank(from) == start_rank);
            } else {
                return test_bit(compute_pawn_attack_bb(from, side), to);
            }
        } else {
            if (to != ep_sq)
                return false;

            if ((ADJ_FILES_BB[ep_sq] & make_bb(from)) == 0)
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
        if (flag != MVEFLAG_NORMAL)
            return false;

        std::uint64_t attacks =
            compute_attack_bb(compute_piece_type(moving), from, get_bb());
        return test_bit(attacks, to);
    }
    return false;
}