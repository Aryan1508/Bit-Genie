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
#include "tt.h"
#include <fstream>
#include <filesystem>
#include <vector>

namespace BenchMark
{
    std::vector<std::string> get_benchmark_positions() 
    {
        std::vector<std::string> fens;
        std::filesystem::path path = std::filesystem::current_path() / "src/benchmark/positions.txt";
        std::ifstream fil(path.string());

        for (std::string fen; std::getline(fil, fen);)
            fens.push_back(fen);

        return fens;
    }

    // Benchmark a perft test and print out the nodes and time taken
	void perft(Position& position, int depth)
	{
		uint64_t nodes = 0;
		StopWatch watch;
		watch.go();
		position.perft(depth, nodes);
		watch.stop();

		std::cout << "\nnodes: " << nodes;
		std::cout << "\ttime: " << watch.elapsed_time().count() << " ms" << std::endl;
	}

    // Run through a list of positions and search a fixed depth at each 
    // position. Print out the total nodes search and the nodes per second ( nodes / time)
	void bench(Position position, TTable& tt) // copy on purpose
	{
        tt.reset();
		StopWatch<> watch;
		watch.go();
		uint64_t nodes = 0;

		for (auto const& fen : get_benchmark_positions())
		{
			if (!position.set_fen(fen))
			{
				std::cout << fen;
				throw std::runtime_error("Invalid fen in bench");
			}

            uint64_t count = bench_search_position(position, tt);
            std::cout << fen << ": " << nodes << std::endl;
			nodes += count; 
		}
		watch.stop();
        
        std::cout << "Time elapsed: " << watch.elapsed_time().count() / 1000.0f << std::endl;

		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(watch.elapsed_time()).count();
		elapsed = std::max(static_cast<int64_t>(1ll), elapsed);

		std::cout << nodes << " nodes " << int((nodes / elapsed)) << " nps" << std::endl;
	}
}