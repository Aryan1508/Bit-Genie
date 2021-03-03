#include "board.h"

Rank get_square_rank(const Square sq) 
{
  return static_cast<Rank>(to_int(sq) % to_int(File::total));
}

File get_square_file(const Square sq)
{
  return static_cast<File>(to_int(sq) / to_int(Rank::total));
}

std::ostream& operator<<(std::ostream& o, const File file)
{
  return o << static_cast<char>((to_int(file) + 98));
}

std::ostream& operator<<(std::ostream& o, const Rank file)
{
  return o << static_cast<char>((to_int(file) + 48));
}