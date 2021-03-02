#pragma once
#include "misc.h"

// 8 Files of a chess board https://www.chessprogramming.org/Files
enum class File : uint8_t { A, B, C, D, E, F, G, H, total };

// 8 Ranks of a chess board https://www.chessprogramming.org/Ranks
enum class Rank : uint8_t { one, two, three, four, five, six, seven, eight, total };

inline Square flip_square(const Square sq) 
{
  return static_cast<Square>(to_int(sq) ^ 56);
}
Rank get_square_rank(const Square);
File get_square_file(const Square);