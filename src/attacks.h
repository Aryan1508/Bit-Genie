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
#pragma once
#include "bitboard.h"
#include "magicmoves.hpp"
#include "position.h"

namespace Attacks {
inline std::uint64_t pawn(std::uint64_t pawns, Color side) {
    std::uint64_t forward = shift(pawns, compute_relative_forward(side));
    return shift(forward, DIR_WEST) | shift(forward, DIR_EAST);
}

inline std::uint64_t pawn(Square sq, Color side) {
    return BitMask::pawn_attacks[side][sq];
}

inline std::uint64_t knight(Square sq) {
    return BitMask::knight_attacks[sq];
}

inline std::uint64_t king(Square sq) {
    return BitMask::king_attacks[sq];
}

inline std::uint64_t bishop(Square sq, std::uint64_t occ) {
    return Bmagic(sq, occ);
}

inline std::uint64_t rook(Square sq, std::uint64_t occ) {
    return Rmagic(sq, occ);
}

inline std::uint64_t queen(Square sq, std::uint64_t occ) {
    return Qmagic(sq, occ);
}

inline bool square_attacked(
    Position const &position, Square sq, Color enemy, std::uint64_t occupancy) {
    std::uint64_t pawns   = position.get_bb(PT_PAWN, enemy);
    std::uint64_t knights = position.get_bb(PT_KNIGHT, enemy);
    std::uint64_t queens  = position.get_bb(PT_QUEEN, enemy);
    std::uint64_t kings   = position.get_bb(PT_KING, enemy);
    std::uint64_t rooks   = position.get_bb(PT_ROOK, enemy) | queens;
    std::uint64_t bishops = position.get_bb(PT_BISHOP, enemy) | queens;

    return (BitMask::pawn_attacks[!enemy][sq] & pawns) ||
           (bishop(sq, occupancy) & bishops) || (rook(sq, occupancy) & rooks) ||
           (knight(sq) & knights) || (king(sq) & kings);
}

inline bool square_attacked(Position const &position, Square sq, Color enemy) {
    return square_attacked(position, sq, enemy, position.get_bb());
}

inline std::uint64_t attackers_to_sq(Position const &position, Square sq) {
    std::uint64_t occ     = position.get_bb();
    std::uint64_t wpawns  = position.get_bb(PCE_WPAWN);
    std::uint64_t bpawns  = position.get_bb(PCE_BPAWN);
    std::uint64_t knights = position.get_bb(PT_KNIGHT);
    std::uint64_t queens  = position.get_bb(PT_QUEEN);
    std::uint64_t kings   = position.get_bb(PT_KING);
    std::uint64_t rooks   = position.get_bb(PT_ROOK) | queens;
    std::uint64_t bishops = position.get_bb(PT_BISHOP) | queens;

    std::uint64_t p_attackers =
        (pawn(sq, CLR_WHITE) & bpawns) | (pawn(sq, CLR_BLACK) & wpawns);

    return p_attackers | (knight(sq) & knights) | (king(sq) & kings) |
           (bishop(sq, occ) & bishops) | (rook(sq, occ) & rooks);
}

inline std::uint64_t generate(PieceType piece, Square sq, std::uint64_t occ) {
    switch (piece) {
    case PT_KNIGHT:
        return knight(sq);
    case PT_BISHOP:
        return bishop(sq, occ);
    case PT_ROOK:
        return rook(sq, occ);
    case PT_QUEEN:
        return queen(sq, occ);
    default:
        return king(sq);
    }
    return 0;
}
} // namespace Attacks