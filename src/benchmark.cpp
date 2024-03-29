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
#include "search.h"
#include "position.h"
#include "stopwatch.h"

#include <iomanip>
#include <cmath>

namespace {
const std::array<std::string, 50> benchmark_fens{
#include "bench.txt"
};

uint64_t perft(Position &position, int depth, bool root = true) {
    Movelist movelist;
    position.generate_legal(movelist);
    uint64_t nodes = 0;

    if (depth == 1)
        return movelist.size();

    for (auto move : movelist) {
        position.apply_move(move);
        uint64_t child = perft(position, depth - 1, false);
        position.revert_move();

        if (root)
            std::cout << move << ": " << child << '\n';
        nodes += child;
    }
    return nodes;
}
}

void perft(Position &position, int depth) {
    StopWatch<> watch;
    watch.go();
    auto nodes = perft(position, depth, true);
    watch.stop();

    long long elapsed      = std::max(1ll, static_cast<long long>((watch.elapsed_time()).count()));
    double elapsed_seconds = elapsed / 1000.0f;

    std::cout << "\nnodes: " << nodes;
    std::cout << "\ttime: " << std::setprecision(2) << std::fixed << elapsed_seconds << " seconds";

    if (elapsed_seconds >= 1)
        std::cout << "\tnps: " << int(nodes / elapsed_seconds);

    std::cout << std::endl;
}

void bench() {
    StopWatch watch;
    watch.go();
    auto nodes = 0;
    for (auto const &fen : benchmark_fens) {
        SearchInfo search;
        search.limits.max_depth = 11;
        search.position.set_fen(fen);
        search_position(search, false);

        std::cout << fen << ": " << search.nodes << '\n';
        nodes += search.nodes;
    }
    watch.stop();
    auto elapsed = watch.elapsed_time().count();
    std::cout << nodes << " nodes " << std::fixed << std::setprecision(0) << std::round(nodes / (elapsed / 1000.0f)) << " nps" << std::endl;
}