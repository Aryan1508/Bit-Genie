#include "Square.h"
#include "board.h"

namespace
{
  bool verify_sq_size(std::string_view sq)
  {
    return sq.size() == 2;
  }

  bool verify_sq_rank(const char rank)
  {
    std::string valid_ranks = "123456789";
    return valid_ranks.find(rank) != std::string::npos;
  }

  bool verify_sq_file(const char file)
  {
    std::string valid_files = "abcdefgh";
    return valid_files.find(file) != std::string::npos;
  }
}
bool is_valid_sq(std::string_view sq) 
{
  return verify_sq_size(sq)
     &&  verify_sq_file(sq[0])
     &&  verify_sq_rank(sq[1]);
}

std::ostream& operator<<(std::ostream& o, const Square sq)
{
  if (sq == Square::bad_sq)
  {
    return o << '-';
  }
  assert(is_ok(sq));
  return o << file_of(sq) << rank_of(sq);
}