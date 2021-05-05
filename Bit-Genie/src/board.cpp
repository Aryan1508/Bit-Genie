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
#include "board.h"
#include "piece.h"
#include "Square.h"

// Return the rank that a square is situated on
Rank rank_of(Square sq)
{
    return static_cast<Rank>(sq >> 3);
}

// Return the file that a square is situated on
File file_of(Square sq)
{
    return static_cast<File>(sq & 7);
}

// Return the rank that a square is situated on relative to its color
// The 8th rank on the board would be the first rank in black's point of view
Rank rank_of(Square sq, Color color)
{
    return color == Black ? rank_of(flip_square(sq)) : rank_of(sq);
}

std::ostream &operator<<(std::ostream &o, File file)
{
    return o << static_cast<char>((to_int(file) + 97));
}

std::ostream &operator<<(std::ostream &o, Rank file)
{
    return o << static_cast<char>((to_int(file) + 49));
}