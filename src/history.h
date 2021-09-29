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
#include "movelist.h"
#include "position.h"

typedef int16_t HistoryTable[2][64][64];
typedef int16_t CounterHistoryTable[12][64][12][64];
typedef Move KillerTable[64][2];
typedef int EvalHistory[64];

inline void add_killer(KillerTable &table, int ply, Move move) {
    table[ply][1] = table[ply][0];
    table[ply][0] = move;
}

inline int16_t &get_history(HistoryTable table, Position const &position, Move move) {
    return table[position.get_side()][move.from()][move.to()];
}

inline int16_t &get_history(CounterHistoryTable table, Position const &position, Move move) {
    auto previous_move  = position.previous_move();
    auto previous_piece = position.get_piece(previous_move.to());
    auto current_piece  = position.get_piece(move.from());
    return table[previous_piece][previous_move.to()][current_piece][move.to()];
}

inline void history_bonus(int16_t &cur, int bonus) {
    cur += 32 * bonus - cur * abs(bonus) / 512;
}

template <typename HistoryType>
inline void history_bonus(HistoryType table, Position const &position, Move move, int depth) {
    history_bonus(get_history(table, position, move), depth * depth);
}

template <typename HistoryType>
inline void update_history(HistoryType table, Position const &position, Move good, Movelist const &other, int depth) {
    auto bonus = depth * depth;

    history_bonus(table, position, good, depth);

    for (auto move : other) {
        if (move == good)
            continue;

        history_bonus(get_history(table, position, move), -bonus);
    }
}