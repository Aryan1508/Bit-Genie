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
#include "utils.h"
#include "history.h"
#include <atomic>

constexpr std::uint8_t MAX_PLY       = 64;
constexpr std::int16_t MIN_EVAL      = -32001;
constexpr std::int16_t MAX_EVAL      = -MIN_EVAL;
constexpr std::int16_t MATE_EVAL     = MAX_EVAL - 1;
constexpr std::int16_t MIN_MATE_EVAL = MATE_EVAL - MAX_PLY;

// clang-format off
struct SearchStats {
    std::uint64_t nodes   = 0;
    std::uint8_t ply      = 0;
    std::uint8_t depth    = 0;
    std::uint8_t seldepth = 0;
};

struct SearchLimits {
    StopWatch<>  stopwatch;
    std::int64_t movetime  = -1;
    std::uint8_t max_depth = 64;
    bool stopped           = false;
    bool time_set          = false;
};

struct SearchResult {
    std::int16_t score      = MIN_EVAL;
    Move best_move          = MOVE_NULL;

    SearchResult(const std::int16_t score, const Move move = MOVE_NULL)   
        : score(score), best_move(move)
    {}

    SearchResult() = default;
};

struct SearchInfo {
public:
    Position     position;
    SearchStats  stats;
    SearchLimits limits;
    EvalTable    eval;
    KilllerTable killers;
    HistoryTable history;
    HistoryTable capture_history;
    CounterHistoryTable counter_history;

public:
    SearchInfo() {
        reset_history_tables();
    }

    void reset_history_tables() {
        reset_history(killers, sizeof(killers));
        reset_history(history, sizeof(history));
        reset_history(capture_history, sizeof(capture_history));
        reset_history(counter_history, sizeof(counter_history));
    }
};

// clang-format on
void init_search_tables();

SearchResult bestmove(SearchInfo &, bool log);

inline std::atomic_bool SEARCH_ABORT = ATOMIC_VAR_INIT(false);