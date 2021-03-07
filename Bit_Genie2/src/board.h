#pragma once
#include "misc.h"
enum class File : uint8_t { A, B, C, D, E, F, G, H, total };
enum class Rank : uint8_t { one, two, three, four, five, six, seven, eight, total };

Rank get_square_rank(Square);
File get_square_file(Square);

std::ostream& operator<<(std::ostream&, File);
std::ostream& operator<<(std::ostream&, Rank);