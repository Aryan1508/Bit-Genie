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

template<typename duration = std::chrono::milliseconds>
class StopWatch
{
	using clock = std::chrono::steady_clock;
	using time_point = std::chrono::time_point<clock, duration>;

	static constexpr time_point current_time() noexcept
	{
		return std::chrono::time_point_cast<duration>(clock::now());
	}
public:
	StopWatch() = default;

	void reset() noexcept
	{
		clock_state = State::idle;
	}

	time_point go() noexcept
	{
		if (clock_state != State::stopped)
			start_point = current_time();

		clock_state = State::running;
		return start_point;
	}

	void stop() noexcept
	{
		if (clock_state == State::running)
		{
			stop_point = current_time();
			clock_state = State::stopped;
		}
	}

	duration elapsed_time() const noexcept
	{
		switch (clock_state)
		{
			case State::idle:
				return std::chrono::duration_cast<duration>(clock::duration::zero());

			case State::running:
				return std::chrono::duration_cast<duration>(current_time() - start_point);

			case State::stopped:
				return std::chrono::duration_cast<duration>(stop_point - start_point);

			default:
				return std::chrono::duration_cast<duration>(clock::duration::zero());
				break;
		}
	}

protected:
	enum class State : unsigned char { idle, running, stopped };

	time_point start_point;
	time_point stop_point;
	State      clock_state = State::idle;
};
#endif // STOPWATCH_H_