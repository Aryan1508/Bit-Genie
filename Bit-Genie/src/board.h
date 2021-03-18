#pragma once
#include "misc.h"
enum class File : uint8_t { A, B, C, D, E, F, G, H, total };
enum class Rank : uint8_t { one, two, three, four, five, six, seven, eight, total };

Rank rank_of(Square);
Rank rank_of(Square, Color);
File file_of(Square);

std::ostream& operator<<(std::ostream&, File);
std::ostream& operator<<(std::ostream&, Rank);