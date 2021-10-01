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
#include "stopwatch.h"

class SearchLimits {
public:
    SearchLimits() {
        reset();
    }

    void reset() {
        stopwatch.reset();
        movetime  = 0;
        max_depth = 64;
        stopped = time_set = false;
    }

    void set_movetime(int64_t);

    void update();

    StopWatch<> stopwatch;
    int64_t movetime;
    int max_depth;
    bool stopped;
    bool time_set;
};