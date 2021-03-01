#include "board.h"

Rank get_square_rank(const Square sq) {
  return static_cast<Rank>(to_underlying(sq) % to_underlying(File::total));
}

File get_square_file(const Square sq) {
  return static_cast<File>(to_underlying(sq) / to_underlying(Rank::total));
}