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
#include "position.h"
#include <array>
#include <algorithm>

class Movelist
{
public:
    using movelist_type = std::array<Move, 256>;
    using iterator = movelist_type::iterator;
    using const_iterator = movelist_type::const_iterator;

    Movelist()
        : cap(0)
    {
    }

    void clear()
    {
        cap = 0;
    }

    iterator begin()
    {
        return moves.begin();
    }

    const_iterator begin() const
    {
        return moves.begin();
    }

    iterator end()
    {
        return moves.begin() + cap;
    }

    const_iterator end() const
    {
        return moves.begin() + cap;
    }

    size_t size() const
    {
        return static_cast<size_t>(cap);
    }

    Move &operator[](size_t pos) { return moves[pos]; }

    void add(Move move)
    {
        moves[cap++] = move;
    }

    bool find(Move move)
    {
        return std::find(begin(), end(), move) != end();
    }

private:
    movelist_type moves;
    std::ptrdiff_t cap;
};