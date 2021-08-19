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
#include "game.h"
#include "../cmdline.h"

#include <thread>
#include <atomic>

namespace Generator
{
    class GamePool
    {
    public:
        static int n_threads;

        static void init(CommandLineParser const& cmd)
        {
            cmd.set_option("-threads", n_threads, 1);
        }

        GamePool() = default;

        void run(std::uint64_t target_fens);

        void run_batch(std::string_view output_file, std::uint64_t target_fens, std::uint64_t seed);
    private:
        std::vector<std::thread> workers;
        std::atomic_uint64_t n_games = {0};
        std::atomic_uint64_t n_fens  = {0};
    };
}