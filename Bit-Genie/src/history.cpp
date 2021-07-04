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
#include "history.h"
#include "movelist.h"
#include "position.h"

int16_t& get_history(HistoryTable table, Position const& position, Move move)
{
    return table[position.side][move_from(move)][move_to(move)];
}

int16_t& get_counter_history(CounterHistory table, Position const& position, Move move)
{
    Move prev_m = position.history.total > 0 ? position.history.previous().move : NullMove;

    if (!prev_m) return table[0][0][0][0];

    Piece pre_p = position.pieces.squares[move_to(prev_m)];
    Piece cur_p = position.pieces.squares[move_from(move)];

    return table[pre_p][0][cur_p][move_to(move)];
}

void counter_history_bonus(CounterHistory table, Position const& position, Move move, int depth)
{
    history_bonus(get_counter_history(table, position, move), depth * depth);
}

void history_bonus(int16_t& cur, int bonus) 
{
    cur += 32 * bonus - cur * abs(bonus) / 512;
}

void history_bonus(HistoryTable table, Position const& position, Move move, int depth)
{
    history_bonus(get_history(table, position, move), depth * depth);
}

void update_history(HistoryTable table, Position const& position, Move good, Movelist const& other, int depth)
{
    int bonus = depth * depth;
    
    history_bonus(table, position, good, depth);

    for(auto move : other)
    {
        if (move_without_score(good) == move_without_score(move)) continue;

        history_bonus(get_history(table, position, move), -bonus);
    }
}