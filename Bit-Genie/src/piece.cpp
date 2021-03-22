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
#include "piece.h"

std::ostream& operator<<(std::ostream& o, PieceType type)
{
  constexpr char labels[]
  {
    'p', 'n', 'b', 'r', 'q', 'k', '.'
  };
  return o << labels[type];
}

std::ostream& operator<<(std::ostream& o, Color color)
{
  constexpr char labels[]
  {
    'w', 'b', '\0'
  };

  return o << labels[color];
}

std::ostream& operator<<(std::ostream& o, Piece piece)
{
  constexpr char labels[]
  {
    'P', 'N', 'B', 'R', 'Q', 'K', 
    'p', 'n', 'b', 'r', 'q', 'k', '.'
  };
  return o << labels[piece];
}
