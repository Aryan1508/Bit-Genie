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
#include "board.h"
#include "move.h"
#include <vector>

enum TTFlag : uint8_t {
    TTFLAG_LOWER,
    TTFLAG_UPPER,
    TTFLAG_EXACT,
    TTFLAG_NULL = 0
};

struct TEntry {
    uint64_t hash = 0;
    int16_t score = 0;
    int16_t seval = 0;
    uint16_t move = 0;
    uint8_t depth = 0;
    TTFlag flag   = TTFLAG_NULL;

    TEntry() = default;

    TEntry(uint64_t h, int16_t s, Move m, uint8_t d, TTFlag fl, int16_t eval)
        : hash(h), score(s), seval(eval), move(m.data), depth(d), flag(fl) {
    }
};

class TTable {
public:
    TTable();

    TTable(int mb) {
        resize(mb);
    }

    void resize(int);

    void add(Position const &, Move, int16_t score, uint8_t depth, TTFlag, int16_t);

    void reset() {
        std::fill(entries.begin(), entries.end(), TEntry());
    }

    TEntry &retrieve(Position const &);

    std::vector<Move> extract_pv(Position &, int);

private:
    std::vector<TEntry> entries;
};

inline TTable TT(8);