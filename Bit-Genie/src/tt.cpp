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
#include "tt.h"
#include "position.h"

static inline int mb_to_b(int mb)
{
    return mb * 1024 * 1024;
}

TTable::TTable()
{
    resize(8);
}

void TTable::resize(int mb)
{
    entries.resize(mb_to_b(mb) / sizeof(TEntry), TEntry());
}

void TTable::add(Position const &position, Move move, int score, uint8_t depth, TEFlag flag, int16_t seval)
{
    uint64_t hash = position.key.data();
    uint64_t index = hash % entries.size(); 

    entries[index] = TEntry(hash, score, move, depth, flag, seval);
}

TEntry &TTable::retrieve(Position const &position)
{
    uint64_t hash = position.key.data();
    uint64_t index = hash % entries.size();
    return entries[index];
}

std::vector<Move> TTable::extract_pv(Position& position, int depth) 
{
    std::vector<Move> pv;
    int distance = 0;
    TEntry *entry = &retrieve(position);

    while (entry->hash == position.key.data() && depth != 0)
    {
        if (position.move_exists((Move)entry->move))
        {
            distance++;
            position.apply_move((Move)entry->move);
            pv.push_back((Move)entry->move);
            depth--;
        }
        else
            break;

        entry = &retrieve(position);
    }

    while(distance--)
        position.revert_move();

    return pv;
}