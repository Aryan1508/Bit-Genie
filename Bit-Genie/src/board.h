/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include "misc.h"
#include <ostream>

enum class File : uint8_t
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    total
};
enum class Rank : uint8_t
{
    one,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight,
    total
};

Rank rank_of(Square);
Rank rank_of(Square, Color);
File file_of(Square);

std::ostream &operator<<(std::ostream &, File);
std::ostream &operator<<(std::ostream &, Rank);