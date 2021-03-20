/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie>

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
#include <iostream>

enum Color : uint8_t
{
	White, Black
};

enum PieceType : uint8_t
{
	Pawn, Knight, Bishop, Rook, Queen, King
};

enum Piece : uint8_t
{
	wPawn, wKnight, wBishop, wRook, wQueen, wKing,
	bPawn, bKnight, bBishop, bRook, bQueen, bKing, Empty
};

std::ostream& operator<<(std::ostream&, PieceType);
std::ostream& operator<<(std::ostream&, Color);
std::ostream& operator<<(std::ostream&, Piece);


inline Piece make_piece(PieceType type, Color color)
{
	return static_cast<Piece>(to_int(type) + (to_int(color) * 6));
}

inline Color color_of(Piece piece)
{
	return Color(bool(((piece + 2) & 0x8)));
}

inline PieceType type_of(Piece piece)
{
	static constexpr PieceType lt[]{
	  Pawn, Knight, Bishop, Rook, Queen, King,
	  Pawn, Knight, Bishop, Rook, Queen, King,
	};
	return lt[piece];
}

inline Color operator!(Color color)
{
	return static_cast<Color>(!to_int(color));
}