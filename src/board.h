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
#include <stdint.h>
#include <iostream>
#include <string_view>

// clang-format off

enum Square : uint8_t {
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
	SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8, SQ_TOTAL, SQ_NULL
};

enum Color : uint8_t {
    CLR_WHITE, CLR_BLACK, CLR_TOTAL
};

enum PieceType : uint8_t {
    PT_PAWN  , PT_KNIGHT,
    PT_BISHOP, PT_ROOK,
    PT_QUEEN , PT_KING, PT_TOTAL
};

enum Piece : uint8_t {
    PCE_WPAWN  , PCE_WKNIGHT,
    PCE_WBISHOP, PCE_WROOK  ,
    PCE_WQUEEN , PCE_WKING  ,

    PCE_BPAWN  , PCE_BKNIGHT,
    PCE_BBISHOP, PCE_BROOK  ,
    PCE_BQUEEN , PCE_BKING  , PCE_TOTAL, PCE_NULL = 12
};

enum Direction : int8_t {
    DIR_NORTH = 8, DIR_SOUTH = -8,
    DIR_EAST  = 1, DIR_WEST  = -1,
};

enum File : uint8_t { 
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_TOTAL
};

enum Rank : uint8_t { 
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_TOTAL 
};

class Position;

// clang-format on

constexpr auto compute_rank(Square sq) {
    return static_cast<Rank>(sq >> 3);
}

constexpr auto flip_square(Square sq) {
    return static_cast<Square>(sq ^ 56);
}

constexpr auto compute_rank(Square sq, Color side) {
    return side == CLR_WHITE ? compute_rank(sq) : compute_rank(flip_square(sq));
}

constexpr auto compute_file(Square sq) {
    return static_cast<File>(sq & 7);
}

constexpr auto compute_relative_forward(Color clr) {
    return clr == CLR_WHITE ? DIR_NORTH : DIR_SOUTH;
}

constexpr auto psqt_sq(Square sq, Color clr) {
    return clr == CLR_WHITE ? flip_square(sq) : sq;
}

constexpr auto operator++(Square &sq, int) {
    const auto pre = sq;
    sq             = static_cast<Square>(sq + 1);
    return pre;
}

constexpr auto operator+=(Square &sq, int delta) {
    sq = static_cast<Square>(sq + delta);
    return sq;
}

constexpr auto make_piece(PieceType pt, Color clr) {
    return static_cast<Piece>(pt + clr * 6);
}

constexpr auto compute_color(Piece pce) {
    return static_cast<Color>(pce / 6);
}

constexpr auto compute_piece_type(Piece pce) {
    return static_cast<PieceType>(pce % 6);
}

constexpr auto operator!(Color clr) {
    return static_cast<Color>(!static_cast<bool>(clr));
}

inline auto &operator<<(std::ostream &o, File file) {
    return o << static_cast<char>(file + 97);
}

inline auto &operator<<(std::ostream &o, Rank rank) {
    return o << static_cast<char>(rank + 49);
}

inline auto &operator<<(std::ostream &o, Square sq) {
    if (sq == SQ_NULL) {
        return o << '-';
    }
    return o << compute_file(sq) << compute_rank(sq);
}

inline auto &operator<<(std::ostream &o, PieceType pt) {
    constexpr char labels[]{ 'p', 'n', 'b', 'r', 'q', 'k', '.' };
    return o << labels[pt];
}

inline auto &operator<<(std::ostream &o, Color clr) {
    constexpr char labels[]{ 'w', 'b' };
    return o << labels[clr];
}

inline auto &operator<<(std::ostream &o, Piece pce) {
    constexpr char labels[]{ 'P', 'N', 'B', 'R', 'Q', 'K', 'p',
                             'n', 'b', 'r', 'q', 'k', '.' };
    return o << labels[pce];
}