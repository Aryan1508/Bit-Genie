#include "position.h"

Position::Position() {
  reset();
}

void Position::reset_squares() {
  squares.fill(Piece());
}

void Position::reset_enpassant() {
  ep_sq = Square::bad;
}

void Position::reset_bitboards() {
  bitboards.fill(0);
  colors.fill(0);
}

void Position::reset() {
  reset_squares();
  reset_bitboards();
  reset_enpassant();
}

ZobristKey Position::hash() const {
  return key;
}