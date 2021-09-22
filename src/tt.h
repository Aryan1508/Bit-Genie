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
#include "move.h"
#include "position.h"

#include <vector>

struct TEntry {
    std::uint64_t hash;
    Score score;
    Score seval;
    std::uint16_t move;
    Depth depth;
    TTFlag flag;
};

inline HashTable<TEntry> TT;

inline void store_tentry(TEntry const &entry) {
    auto &current = TT.probe(entry.hash);
    if (entry.flag != TTFLAG_EXACT && entry.hash == current.hash && entry.depth < current.depth - 1) {
        return;
    }

    if (entry.flag == TTFLAG_EXACT || entry.depth * 3 > current.depth) {
        current = entry;
    }
}
