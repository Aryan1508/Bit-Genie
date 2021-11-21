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
#include "searchlimits.h"

#include <atomic>
#include <string.h>

struct SearchInfo {
    Position position;
    SearchLimits limits;

    // History-tables
    KillerTable killers                 = { MOVE_NULL };
    EvalHistory eval                    = { 0 };
    HistoryTable history                = { 0 };
    HistoryTable capture_history        = { 0 };
    CounterHistoryTable counter_history = { 0 };

    // Counters
    uint64_t nodes = 0;
    int seldepth   = 0;
    int ply        = 0;

    void reset_for_search() {
        limits.reset();
        memset(killers, 0, sizeof(killers));
        memset(eval, 0, sizeof(eval));
        memset(history, 0, sizeof(history));
        memset(capture_history, 0, sizeof(capture_history));
        memset(counter_history, 0, sizeof(counter_history));
        nodes = seldepth = ply = 0;
    }

    void copy_history_tables(SearchInfo const &other) {
        memcpy(history, other.history, sizeof(history));
        memcpy(capture_history, other.capture_history, sizeof(capture_history));
        memcpy(counter_history, other.counter_history, sizeof(counter_history));
    }
};

void init_search_tables();

void search_position(SearchInfo &, bool log);

inline std::atomic_bool SEARCH_ABORT = ATOMIC_VAR_INIT(false);