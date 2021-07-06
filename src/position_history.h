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
#include <array>
#include "misc.h"
#include "move.h"
#include "zobrist.h"

class PositionHistory
{
public:
    struct Undo
    {
        int half_moves = 0;
        Square ep_sq = Square::bad_sq;
        CastleRights castle;
        ZobristKey key;
        Piece captured;
        Move move;
    };

    PositionHistory()
    {
        total = 0;
    }

    Undo &current()
    {
        return history[total];
    }

    Undo const &previous() const
    {
        return history[total - 1];
    }

    void reset()
    {
        total = 0;
    }

    bool is_drawn(ZobristKey current) const
    {
        for (int i = 0; i < total; i++)
        {
            if (history[i].key == current)
                return true;
        }
        return false;
    }

    int total = 0;

private:
    std::array<Undo, 2048> history;
};