#include "Square.h"

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

// A string square always has 2 characters ex. `e4`
// The first character is always a file (a, b, c..., h)
// The second character is always a rank(1, 2, 3..., 8)
bool is_valid_sq(std::string_view sq) 
{
  return verify_sq_size(sq)
     &&  verify_sq_file(sq[0])
     &&  verify_sq_rank(sq[1]);
}