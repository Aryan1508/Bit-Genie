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
#include "misc.h"

typedef int16_t HistoryTable[2][64][64]; 
typedef int16_t CounterHistory[12][64][12][64];

int16_t& get_history(HistoryTable, Position const&, Move);
int16_t& get_counter_history(CounterHistory, Position const&, Move);

void history_bonus(int16_t& cur, int bonus);
void history_bonus(HistoryTable, Position const&, Move, int);
void counter_history_bonus(CounterHistory, Position const&, Move, int);
void update_history(HistoryTable, Position const&, Move, Movelist const& other, int depth);