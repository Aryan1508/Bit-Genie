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
#include "tt.h"
#include "uci.h"
#include "utils.h"
#include "search.h"
#include "uciparse.h"
#include "position.h"
#include "benchmark.h"
#include "searchinit.h"

#include <cstring>
#include <algorithm>

const std::string version = "9.01";

namespace {
void uci_ok() {
    std::cout << "id name Bit-Genie " << version << '\n';
    std::cout << "id author Aryan Parekh" << '\n';
    std::cout << "id network " << std::hex << Network::get_hash() << std::dec << '\n';
    std::cout << "option name Hash type spin default 8 min 2 max 3000" << '\n';
    std::cout << "option name Clear Hash type button" << '\n';
    std::cout << "uciok" << std::endl;
}

void uci_ready() {
    std::cout << "readyok" << std::endl;
}

void uci_setoption(UciParser const &parser) {
    auto [name, value] = parser.parse_setoption();

    if (name == "hash") {
        TT.resize(std::stoi(value));
    }

    else if (name == "clear hash")
        TT.reset();
}

void uci_stop(SearchInit &worker) {
    if (worker.is_searching())
        worker.end();
}

void uci_go(UciParser const &parser, Position &position, SearchInit &worker) {
    UciGo options = parser.parse_go(position.get_side());

    Search::Info &search = *worker.search;

    search          = Search::Info();
    search.position = &position;
    search.limits.stopwatch.go();
    search.limits.max_depth = std::min(options.depth, 64);

    if (options.movetime == -1) {
        auto &t   = position.get_side() == CLR_WHITE ? options.wtime : options.btime;
        auto &inc = position.get_side() == CLR_WHITE ? options.winc : options.binc;

        if (t == -1)
            search.limits.movetime = std::numeric_limits<int64_t>::max();

        else {
            search.limits.time_set = true;
            search.limits.movetime = t / options.movestogo + inc - 50;
        }
    } else {
        search.limits.movetime = options.movetime - 50;
        search.limits.time_set = true;
    }

    worker.begin();
}

void uci_setposition(UciParser const &parser, Position &position) {
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
} // namespace

namespace UCI {
void init(int argc, char **argv) {
    UciParser command;
    Position position;
    SearchInit worker;

    if (argc > 1 && !strncmp(argv[1], "bench", 5)) {
        run_bench(position);
        return;
    }

    while (command.take_input()) {
        if (command == UciCommands::quit) {
            uci_stop(worker);
            break;
        }

        else if (command == UciCommands::isready)
            uci_ready();

        else if (command == UciCommands::uci)
            uci_ok();

        else if (command == UciCommands::position)
            uci_setposition(command, position);

        else if (command == UciCommands::print)
            std::cout << position << std::endl;

        else if (command == UciCommands::perft)
            run_perft(position, command.parse_perft());

        else if (command == UciCommands::go)
            uci_go(command, position, worker);

        else if (command == UciCommands::stop)
            uci_stop(worker);

        else if (command == UciCommands::setoption)
            uci_setoption(command);

        else if (command == UciCommands::bench) {
            TT.reset();
            run_bench(position);
        }

        else if (command == UciCommands::ucinewgame) {
            uci_stop(worker);
            TT.reset();
        }
    }
}
} // namespace UCI