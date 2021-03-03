#include "move.h"
#include "Square.h"
#include <string>

Move::Move()
  : data(0)
{}

Square Move::from() const
{
  return static_cast<Square>(data & 0x3f);
}

Square Move::to() const
{
  return static_cast<Square>((data >> 6) & 0x3f);
}

Move::GenType Move::type() const
{
  return static_cast<GenType>((data >> 12) & 0x3);
}

Piece::Type Move::promoted() const
{
  return static_cast<Piece::Type>((data >> 14) & 0b11);
}

std::ostream& operator<<(std::ostream& o, const Move move)
{
  constexpr char prom_piece_labels[]{ 'n', 'b', 'r', 'q' };

  o << move.from() << move.to();

  if (move.type() == Move::promotion)
  {
    o << prom_piece_labels[move.promoted()];
  }
  return o;
}