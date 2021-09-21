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
#include "history.h"
#include "move.h"
#include "searchlimits.h"
#include "searchstats.h"
#include <atomic>

namespace Search {
struct Info {
    Position *position;
    Stats stats;
    Limits limits;
    Move killers[64][2]                 = { NullMove };
    HistoryTable history                = { 0 };
    HistoryTable capture_history        = { 0 };
    CounterHistoryTable counter_history = { 0 };

    int eval[64] = { 0 };

    void update();
};

void init();
std::uint64_t bestmove(Info &, bool log);
} // namespace Search

inline std::atomic_bool SEARCH_ABORT = ATOMIC_VAR_INIT(false);