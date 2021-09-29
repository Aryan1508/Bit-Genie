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
#ifndef STOPWATCH_H_
#define STOPWATCH_H_
#include <chrono>

template <typename duration = std::chrono::milliseconds>
class StopWatch {
    using clock      = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock, duration>;

    static constexpr time_point current_time() noexcept {
        return std::chrono::time_point_cast<duration>(clock::now());
    }

public:
    StopWatch() = default;

    void reset() noexcept {
        clock_state = STATE_IDLE;
    }

    time_point go() noexcept {
        if (clock_state != STATE_STOPPED)
            start_point = current_time();

        clock_state = STATE_RUNNING;
        return start_point;
    }

    void stop() noexcept {
        if (clock_state == STATE_RUNNING) {
            stop_point  = current_time();
            clock_state = STATE_STOPPED;
        }
    }

    duration elapsed_time() const noexcept {
        switch (clock_state) {
        case STATE_IDLE:
            return std::chrono::duration_cast<duration>(clock::duration::zero());

        case STATE_RUNNING:
            return std::chrono::duration_cast<duration>(current_time() - start_point);

        case STATE_STOPPED:
            return std::chrono::duration_cast<duration>(stop_point - start_point);

        default:
            return std::chrono::duration_cast<duration>(clock::duration::zero());
            break;
        }
    }

protected:
    enum State : uint8_t {
        STATE_IDLE,
        STATE_RUNNING,
        STATE_STOPPED
    };

    time_point start_point;
    time_point stop_point;
    State clock_state = STATE_IDLE;
};
#endif // STOPWATCH_H_