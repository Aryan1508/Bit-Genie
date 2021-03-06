#include "board.h"

Rank get_square_rank(const Square sq) 
{
  return static_cast<Rank>(sq / to_int(File::total));
}

File get_square_file(const Square sq)
{
  return static_cast<File>(sq % to_int(Rank::total));
}

std::ostream& operator<<(std::ostream& o, const File file)
{
  return o << static_cast<char>((to_int(file) + 97));
}

std::ostream& operator<<(std::ostream& o, const Rank file)
{
  return o << static_cast<char>((to_int(file) + 49));
}