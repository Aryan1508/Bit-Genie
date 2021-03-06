#pragma once
#include "misc.h"
#include <iostream>

enum class Color : uint8_t
{
  white, black
};

enum class PieceType : uint8_t
{
  pawn, knight, bishop, rook, queen, king
};

enum class Piece : uint8_t 
{
  wpawn, wknight, wbishop, wrook, wqueen, qking,
  bpawn, bknight, bbishop, brook, bqueen, bking, empty
};

std::ostream& operator<<(std::ostream&, PieceType);
std::ostream& operator<<(std::ostream&, Color);
std::ostream& operator<<(std::ostream&, Piece);


inline Piece make_piece(PieceType type, Color color)
{
  return static_cast<Piece>(to_int(type) + (to_int(color) * 6));
}

inline Color get_piece_color(Piece piece)
{
  return static_cast<Color>(to_int(piece) / 6);
}

inline PieceType get_piece_type(Piece piece)
{
  return static_cast<PieceType>(to_int(piece) % 6);
}

inline Color operator!(Color color)
{
  return static_cast<Color>(!to_int(color));
}