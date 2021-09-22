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
#include "uci.h"
#include "utils.h"
#include "search.h"
#include "threads.h"
#include "uciparse.h"
#include "position.h"
#include "benchmark.h"

#include <cstring>
#include <algorithm>

const std::string version = "9.01";

inline void uci_ok() {
    std::cout << "id name Bit-Genie " << version << '\n';
    std::cout << "id author Aryan Parekh" << '\n';
    std::cout << "id network " << std::hex << Network::get_hash() << std::dec << '\n';
    std::cout << "option name Hash type spin default 8 min 2 max 3000" << '\n';
    std::cout << "option name Clear Hash type button" << '\n';
    std::cout << "uciok" << std::endl;
}

inline void uci_ready() {
    std::cout << "readyok" << std::endl;
}

inline void uci_setoption(UciParser const &parser) {
    auto [name, value] = parser.parse_setoption();

    if (name == "hash") {
        TT.resize(std::stoi(value));
    }

    else if (name == "clear hash")
        TT.reset();
}

inline void uci_go(UciParser const &parser, Position &position, SearchThreadManager &worker) {
    UciGo options = parser.parse_go(position.get_side());

    SearchLimits limits;
    limits.stopwatch.go();
    limits.max_depth = std::min(options.depth, 64);

    if (options.movetime == -1) {
        auto &t   = position.get_side() == CLR_WHITE ? options.wtime : options.btime;
        auto &inc = position.get_side() == CLR_WHITE ? options.winc : options.binc;

        if (t == -1)
            limits.movetime = std::numeric_limits<int64_t>::max();

        else {
            limits.time_set = true;
            limits.movetime = t / options.movestogo + inc - 50;
        }
    } else {
        limits.movetime = options.movetime - 50;
        limits.time_set = true;
    }

    worker.set_limits(limits);
    worker.start_new_search();
}

inline void uci_setposition(UciParser const &parser, Position &position) {
    auto [fen, moves] = parser.parse_position_command();

    position.set_fen(fen);

    for (std::string const &move : moves) {
        Movelist movelist;
        position.generate_legal(movelist);

        for (auto m : movelist)
            if (m.to_str() == move)
                position.apply_move(m);
    }
}

inline void init_uci(int argc, char **argv) {
    if (argc > 1 && !strncmp(argv[1], "bench", 5)) {
        run_bench();
        return;
    }

    UciParser command;
    SearchThreadManager worker;
    auto &position = worker.get_position();

    while (command.take_input()) {
        if (command == UCICMD_QUIT) {
            worker.end();
            break;
        }

        else if (command == UCICMD_ISREADY)
            uci_ready();

        else if (command == UCICMD_UCI)
            uci_ok();

        else if (command == UCICMD_POSITION)
            uci_setposition(command, position);

        else if (command == UCICMD_PRINT)
            std::cout << position << std::endl;

        else if (command == UCICMD_PERFT)
            run_perft(position, command.parse_perft());

        else if (command == UCICMD_GO)
            uci_go(command, position, worker);

        else if (command == UCICMD_STOP)
            worker.end();

        else if (command == UCICMD_SETOPTION)
            uci_setoption(command);

        else if (command == UCICMD_BENCH) {
            TT.reset();
            run_bench();
        }

        else if (command == UCICMD_NEWGAME) {
            worker.end();
            TT.reset();
        }
    }
}