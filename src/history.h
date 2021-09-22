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

#include <cstring>

typedef Score HistoryTable[2][64][64];
typedef Score CounterHistoryTable[12][64][12][64];
typedef Move KilllerTable[64][12];
typedef Score EvalTable[64];

inline auto &get_history(HistoryTable table, Position const &position, Move move) {
    return table[position.get_side()][move.get_from()][move.get_to()];
}

inline auto &get_history(CounterHistoryTable table, Position const &position, Move move) {
    Move previous_move   = position.previous_move();
    Piece previous_piece = position.get_piece(previous_move.get_to());
    Piece current_piece  = position.get_piece(move.get_from());
    return table[previous_piece][previous_move.get_to()][current_piece][move.get_to()];
}

inline void history_bonus(int16_t &cur, int bonus) {
    cur += 32 * bonus - cur * abs(bonus) / 512;
}

template <typename HistoryType>
inline void history_bonus(HistoryType table, Position const &position, Move move, int depth) {
    history_bonus(get_history(table, position, move), depth * depth);
}

template <typename HistoryType>
inline void reset_history(HistoryType table, std::size_t size) {
    std::memset(&table[0], 0, size);
}

template <typename HistoryType>
inline void update_history(HistoryType table, Position const &position, Move good, Movelist const &other, int depth) {
    int bonus = depth * depth;

    history_bonus(table, position, good, depth);

    for (auto move : other) {
        if (move == good)
            continue;

        history_bonus(get_history(table, position, move), -bonus);
    }
}