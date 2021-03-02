#include "piece.h"
#include "misc.h"

Piece::Color Piece::color() const
{
  return static_cast<Color>(data >> 3);
}

Piece::Type Piece::type() const
{
  return static_cast<Type>(data & 0b111);
}

Piece::Piece()
  : data(empty)
{}

Piece::Piece(const Type type, const Color color)
  : data(type | (color << 3))
{}

bool Piece::is_empty() const
{
  return data == empty;
}

std::ostream& operator<<(std::ostream& o, const Piece piece) 
{
  static constexpr const char* color_labels[]
  {
    "White", "Black"
  };
  static constexpr const char* type_labels[]
  {
    "Pawn", "Knight", "Bishop", "Rook", "Queen", "King"
  };
  if (piece.is_empty()) return o << '-';
  return o << color_labels[piece.color()] << ' ' << type_labels[piece.type()];
}