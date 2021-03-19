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