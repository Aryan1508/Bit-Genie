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
#include "Square.h"
#include <ostream>

enum class File : uint8_t
{
    A, B, C,
    D, E, F,
    G, H, total
};

enum class Rank : uint8_t
{
    one,   two,   three,
    four,  five,  six,
    seven, eight, total
};

constexpr Rank rank_of(Square sq) noexcept
{
    return static_cast<Rank>(sq >> 3);
}

constexpr File file_of(Square sq) noexcept
{
    return static_cast<File>(sq & 7);
}

constexpr Rank rank_of(Square sq, Color color) noexcept
{
    return color == Black ? rank_of(flip_square(sq)) : rank_of(sq);
}

std::ostream &operator<<(std::ostream &, File);
std::ostream &operator<<(std::ostream &, Rank);