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
#include "history.h"
#include "searchstats.h"
#include "searchlimits.h"
#include <atomic>

struct SearchInfo {
    Position *position;
    SearchStats stats;
    SearchLimits limits;
    KillerTable killers                 = { MOVE_NULL };
    EvalHistory eval                    = { 0 };
    HistoryTable history                = { 0 };
    HistoryTable capture_history        = { 0 };
    CounterHistoryTable counter_history = { 0 };

    void update();
};

void init_search_tables();

void search_position(SearchInfo &, bool log);

inline std::atomic_bool SEARCH_ABORT = ATOMIC_VAR_INIT(false);