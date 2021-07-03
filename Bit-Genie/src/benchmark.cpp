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
#include "benchmark.h"
#include "position.h"
#include "stopwatch.h"
#include "search.h"
#include <iomanip>

// Benchmark positions from Halogen
const std::array<std::string, 50> benchmark_fens
{
    #include "bench.txt"
};

namespace BenchMark
{
    // Benchmark a perft test and print out the nodes and time taken
    void perft(Position &position, int depth)
    {
        uint64_t nodes = 0;
        StopWatch<> watch;
        watch.go();
        position.perft(depth, nodes);
        watch.stop();

        long long elapsed = std::max(1ll, static_cast<long long>((watch.elapsed_time()).count()));
        double elapsed_seconds = elapsed / 1000.0f;

        std::cout << "\nnodes: " << nodes;
        std::cout << "\ttime: " << std::setprecision(2) << std::fixed << elapsed_seconds << " seconds";
        
        if (elapsed_seconds >= 1)
            std::cout << "\tnps: " << int(nodes / elapsed_seconds);
        
        std::cout << std::endl;
    }

    // Run through a list of positions and search a fixed depth at each
    // position. Print out the total nodes search and the nodes per second ( nodes / time)
    void bench(Position position) // copy on purpose
    {
        StopWatch<> watch;
        watch.go();
        uint64_t nodes = 0;
        for (auto const &fen : benchmark_fens)
        {
            if (!position.set_fen(fen))
            {
                std::cout << fen;
                throw std::runtime_error("Invalid fen in bench");
            }
            Search::Info info;
            info.position = &position;
            info.limits.max_depth = 13;

            auto count = Search::bestmove(info, false);
            std::cout << fen << ": " << count << '\n';
            nodes += count;
        }
        watch.stop();

        std::cout << "Time elapsed: " << watch.elapsed_time().count() / 1000.0f << std::endl;

        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(watch.elapsed_time()).count();
        elapsed = std::max(static_cast<int64_t>(1ll), elapsed);

        std::cout << nodes << " nodes " << int((nodes / elapsed)) << " nps" << std::endl;
    }
}