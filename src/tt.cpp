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
#include <algorithm>

static inline size_t mb_to_b(size_t mb) {
    return mb * 1024 * 1024;
}

TTable::TTable() {
    resize(8);
}

void TTable::resize(size_t mb) {
    entries.resize(mb_to_b(mb) / sizeof(TEntry), TEntry());
}

void TTable::add(Position const &position, Move move, int16_t score, uint8_t depth, TTFlag flag, int16_t seval) {
    auto hash  = position.get_key();
    auto index = hash % entries.size();

    if (flag != TTFLAG_EXACT && hash == entries[index].hash && depth < entries[index].depth - 1)
        return;

    if (flag == TTFLAG_EXACT || depth * 3 > entries[index].depth)
        entries[index] = TEntry(hash, score, move, depth, flag, seval);
}

TEntry &TTable::retrieve(Position const &position) {
    auto hash  = position.get_key();
    auto index = hash % entries.size();
    return entries[index];
}

std::vector<Move> TTable::extract_pv(Position &position, int depth) {
    auto move_exists =
        [&](Move move) {
            Movelist movelist;
            position.generate_legal(movelist);
            return std::find(movelist.begin(), movelist.end(), move) != movelist.end();
        };

    std::vector<Move> pv;
    auto distance = 0;
    auto entry    = &retrieve(position);

    while (entry->hash == position.get_key() && depth != 0) {
        Move pv_move = Move(entry->move);
        if (move_exists(pv_move)) {
            distance++;
            position.apply_move(pv_move);
            pv.push_back(pv_move);
            depth--;
        } else
            break;
        entry = &retrieve(position);
    }
    while (distance--) {
        position.revert_move();
    }
    return pv;
}