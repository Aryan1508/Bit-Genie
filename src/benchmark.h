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
#include "utils.h"
#include "search.h"
#include "position.h"

#include <cmath>
#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>

inline std::uint64_t run_perft(Position &position, std::uint8_t depth, bool root, std::ostream &out) {
    Movelist movelist;
    position.generate_legal(movelist);
    std::uint64_t nodes = 0;

    if (depth == 1)
        return movelist.size();

    for (auto move : movelist) {
        position.apply_move(move);
        const auto child_nodes = run_perft(position, depth - 1, false, out);
        position.revert_move();

        if (root)
            out << move << ": " << child_nodes << '\n';
        nodes += child_nodes;
    }
    return nodes;
}

void run_perft(Position &position, std::uint8_t depth, std::ostream &out = std::cout) {
    StopWatch watch;

    watch.go();
    const auto nodes = run_perft(position, depth, true, out);
    watch.stop();

    const auto elapsed         = std::max<std::int64_t>(1, watch.elapsed_time().count());
    const auto elapsed_seconds = elapsed / 1000.0f;

    out << "\nnodes: " << nodes;
    out << "\ttime: " << std::setprecision(2) << std::fixed
        << elapsed_seconds << " seconds";

    if (elapsed_seconds >= 1)
        out << "\tnps: " << int(nodes / elapsed_seconds);

    out << std::endl;
}

void run_bench(Position &position) {
    StopWatch watch;
    std::uint64_t nodes = 0;

    std::ifstream bench_file("bench.txt");

    if (!bench_file)
        throw std::runtime_error("Couldn't find bench.txt to run benchmark");

    watch.go();
    for (std::string fen; std::getline(bench_file, fen);) {
        position.set_fen(fen);
        Search::Info info;
        info.position         = &position;
        info.limits.max_depth = 11;

        auto count = Search::bestmove(info, false);
        std::cout << fen << ": " << count << '\n';
        nodes += count;
    }
    watch.stop();

    long long elapsed = watch.elapsed_time().count();

    std::cout << nodes << " nodes " << std::fixed << std::setprecision(0)
              << std::round(nodes / (elapsed / 1000.0f)) << " nps" << std::endl;
}