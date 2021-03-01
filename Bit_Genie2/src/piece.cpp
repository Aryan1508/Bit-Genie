#include "piece.h"
#include "misc.h"

Piece::Color Piece::color() const {
  return static_cast<Color>(data >> 3);
}

Piece::Type Piece::type() const {
  return static_cast<Type>(data & 0b111);
}

Piece::Piece()
  : data(empty)
{}

Piece::Piece(const Type type, const Color color)
  : data(type | (color << 3))
{}