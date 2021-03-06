#include "board.h"

Rank get_square_rank(Square sq) 
{
  return static_cast<Rank>(sq / to_int(File::total));
}

File get_square_file(Square sq)
{
  return static_cast<File>(sq % to_int(Rank::total));
}

std::ostream& operator<<(std::ostream& o, File file)
{
  return o << static_cast<char>((to_int(file) + 97));
}

std::ostream& operator<<(std::ostream& o, Rank file)
{
  return o << static_cast<char>((to_int(file) + 49));
}