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
#include "tt.h"
#include "move.h"
#include "history.h"
#include "searchlimits.h"

#include <atomic>
#include <string.h>

inline std::atomic_bool SEARCH_ABORT = ATOMIC_VAR_INIT(false);

constexpr int MIN_EVAL      = -32001;
constexpr int MAX_EVAL      = -MIN_EVAL;
constexpr int MATE_EVAL     = 32000;
constexpr int MAX_PLY       = 64;
constexpr int MIN_MATE_EVAL = MATE_EVAL - MAX_PLY;

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

#ifdef FEN_GENERATOR 
    TTable local_tt = TTable(8);
#endif  

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

struct SearchResult {
    int score      = MIN_EVAL;
    Move best_move = MOVE_NULL;

    SearchResult() = default;

    SearchResult(int best_score, Move best = MOVE_NULL)
        : score(best_score), best_move(best) {
    }
};

void init_search_tables();

int qsearch(SearchInfo &search, int alpha, int beta);

SearchResult search_position(SearchInfo &, bool log);

#ifndef FEN_GENERATOR 
inline void add_tt_entry(SearchInfo&, TEntry const& entry) {
    TT.add(entry);
}

inline TEntry& retrieve_tt_entry(SearchInfo& search) {
    return TT.retrieve(search.position);
}
#else 
inline void add_tt_entry(SearchInfo& search, TEntry const& entry) {
    search.local_tt.add(entry);
}

inline TEntry& retrieve_tt_entry(SearchInfo& search) {
    return search.local_tt.retrieve(search.position);
}

inline uint64_t FEN_GENERATOR_DEPTH = 10;
inline uint64_t FEN_GENERATOR_NODES = 4000;
#endif