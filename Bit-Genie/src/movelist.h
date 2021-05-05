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

    template <bool check = false>
    void add(Position &position, Move &&move)
    {
        if constexpr (check)
        {
            if (!position.move_is_legal(move))
                return;
        }
        moves[cap++] = std::move(move);
    }

    bool find(Move move)
    {
        for (auto m : *this)
        {
            if (move_without_score(m) == move_without_score(move))
                return true;
        }
        return false;
    }

private:
    movelist_type moves;
    std::ptrdiff_t cap;
};