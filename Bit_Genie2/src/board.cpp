#include "board.h"

Rank get_square_rank(const Square sq) 
{
  return static_cast<Rank>(to_int(sq) % to_int(File::total));
}

File get_square_file(const Square sq)
{
  return static_cast<File>(to_int(sq) / to_int(Rank::total));
}