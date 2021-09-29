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
#include "position.h"
#include "magicmoves.hpp"

inline uint64_t generate_pawn_attacks_bb(Square sq, Color side) {
    return PAWN_ATTACKS_BB[side][sq];
}

inline uint64_t generate_knight_attacks_bb(Square sq) {
    return KNIGHT_ATTACKS_BB[sq];
}

inline uint64_t generate_king_attacks_bb(Square sq) {
    return KING_ATTACKS_BB[sq];
}

inline uint64_t generate_bishop_attacks_bb(Square sq, uint64_t occ) {
    return Bmagic(sq, occ);
}

inline uint64_t generate_rook_attacks_bb(Square sq, uint64_t occ) {
    return Rmagic(sq, occ);
}

inline uint64_t generate_queen_attacks_bb(Square sq, uint64_t occ) {
    return Qmagic(sq, occ);
}

inline bool square_is_attacked(Position const &position, Square sq, Color enemy, uint64_t occupancy) {
    auto pawns   = position.get_bb(PT_PAWN, enemy);
    auto knights = position.get_bb(PT_KNIGHT, enemy);
    auto queens  = position.get_bb(PT_QUEEN, enemy);
    auto kings   = position.get_bb(PT_KING, enemy);
    auto rooks   = position.get_bb(PT_ROOK, enemy) | queens;
    auto bishops = position.get_bb(PT_BISHOP, enemy) | queens;
    return (PAWN_ATTACKS_BB[!enemy][sq] & pawns) || (generate_bishop_attacks_bb(sq, occupancy) & bishops) || (generate_rook_attacks_bb(sq, occupancy) & rooks) || (generate_knight_attacks_bb(sq) & knights) || (generate_king_attacks_bb(sq) & kings);
}

inline bool square_is_attacked(Position const &position, Square sq, Color enemy) {
    return square_is_attacked(position, sq, enemy, position.get_bb());
}

inline uint64_t generate_sq_attackers_bb(Position const &position, Square sq) {
    auto occ         = position.get_bb();
    auto wpawns      = position.get_bb(PCE_WPAWN);
    auto bpawns      = position.get_bb(PCE_BPAWN);
    auto knights     = position.get_bb(PT_KNIGHT);
    auto queens      = position.get_bb(PT_QUEEN);
    auto kings       = position.get_bb(PT_KING);
    auto rooks       = position.get_bb(PT_ROOK) | queens;
    auto bishops     = position.get_bb(PT_BISHOP) | queens;
    auto p_attackers = (generate_pawn_attacks_bb(sq, CLR_WHITE) & bpawns) | (generate_pawn_attacks_bb(sq, CLR_BLACK) & wpawns);
    return p_attackers | (generate_knight_attacks_bb(sq) & knights) | (generate_king_attacks_bb(sq) & kings) | (generate_bishop_attacks_bb(sq, occ) & bishops) | (generate_rook_attacks_bb(sq, occ) & rooks);
}

inline uint64_t generate_attacks_bb(PieceType piece, Square sq, uint64_t occ) {
    switch (piece) {
    case PT_KNIGHT:
        return generate_knight_attacks_bb(sq);
    case PT_BISHOP:
        return generate_bishop_attacks_bb(sq, occ);
    case PT_ROOK:
        return generate_rook_attacks_bb(sq, occ);
    case PT_QUEEN:
        return generate_queen_attacks_bb(sq, occ);
    default:
        return generate_king_attacks_bb(sq);
    }
    return 0;
}