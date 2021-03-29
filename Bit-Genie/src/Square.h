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
#include "misc.h"
#include <string_view>

enum Square : uint8_t
{
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	bad_sq = 100,
};

enum class Direction : int8_t
{
	north = 8, south = -8,
	east = 1, west = -1,
};

template<typename E>
inline Square to_sq(E const& e)
{
	return static_cast<Square>(e);
}

template<>
inline Square to_sq<std::string_view>(std::string_view const& sq)
{
	return static_cast<Square>((sq[0] - 97) + ((sq[1] - 49) * 8));
}

inline Direction operator+(Direction l, Direction r)
{
	return static_cast<Direction>(to_int(l) + to_int(r));
}

inline Square operator-(Square l, Direction r)
{
	return static_cast<Square>(to_int(l) - to_int(r));
}

inline Square flip_square(const Square sq)
{
	return to_sq(sq ^ 56);
}

inline Square operator++(Square& sq, int)
{
	Square temp = sq;
	sq = to_sq(sq + 1);
	return temp;
}

inline Square operator+(Square sq, Direction dir)
{
	return to_sq(sq + to_int(dir));
}

inline Square& operator+=(Square& sq, int inc)
{
	sq = to_sq(sq + inc);
	return sq;
}

inline bool is_ok(const Square sq)
{
	return sq >= Square::A1 && sq <= Square::H8;
}
bool is_valid_sq(std::string_view);
std::ostream& operator<<(std::ostream&, const Square);