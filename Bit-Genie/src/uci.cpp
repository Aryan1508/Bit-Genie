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
#include "uci.h"
#include "position.h"
#include "search.h"
#include "uciparse.h"
#include "tt.h"
#include "eval.h"
#include "stringparse.h"
#include "benchmark.h"
#include "searchinit.h"

const char *version = "5.4";

namespace
{
    template <typename... Args>
    void printl(Args const &...args)
    {
        (std::cout << ... << args) << std::endl;
    }

    void uci_ok()
    {
        printl("id name Bit-Genie ", version);
        printl("id author Aryan Parekh");
        printl("option name Hash type spin default 2 min 2 max 3000");
        printl("option name Clear Hash type button");
        printl("uciok");
    }

    void uci_ready()
    {
        printl("readyok");
    }

    void uci_setoption(UciParser const &parser, TTable &tt)
    {
        auto [name, value] = parser.parse_setoption();

        if (name == "hash")
        {
            if (!string_is_number(value))
                return;
            tt.resize(std::stoi(value));
        }

        else if (name == "clear hash")
            tt.reset();
    }

    void uci_stop(SearchInit &worker)
    {
        if (worker.is_searching())
            worker.end();
    }

    void uci_go(UciParser const &parser, Position &position, TTable &tt, SearchInit &worker)
    {
        UciGo options = parser.parse_go(position.side);

        Search search;
        search.limits.stopwatch.go();
        search.limits.max_depth = std::min(options.depth, 64);
        search.limits.stopped = false;
        search.limits.time_set = false;

        if (options.movetime == -1)
        {
            auto &t = position.side == White ? options.wtime : options.btime;

            if (t == -1)
                search.limits.movetime = std::numeric_limits<int64_t>::max();

            else
            {
                search.limits.time_set = true;
                search.limits.movetime = t / options.movestogo - 50;
            }
        }
        else
        {
            search.limits.movetime = options.movetime - 50;
            search.limits.time_set = true;
        }

        worker.begin(search, position, tt);
    }

    void uci_setposition(UciParser const &parser, Position &position)
    {
        auto [fen, moves] = parser.parse_position_command();

        if (!position.set_fen(fen))
        {
            std::cout << fen << std::endl;
            std::cout << "Invalid FEN string" << std::endl;
            return;
        }

        Position temp = position;

        temp.history.total = 0;
        for (std::string const &move : moves)
        {
            if (!temp.apply_move(move))
            {
                std::cout << "Invalid move in movelist: " << move << std::endl;
                continue;
            }
        }

        position = temp;
    }
}

void uci_input_loop(int argc, char **argv)
{
    printl("Bit-Genie by Aryan Parekh");

    UciParser command;
    Position position;
    TTable table(2);
    SearchInit worker;

    if (argc > 1 && !strncmp(argv[1], "bench", 5))
    {
        BenchMark::bench(position, table);
        return;
    }

    while (command.take_input())
    {
        if (command == UciCommands::quit)
        {
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
            printl(position);

        else if (command == UciCommands::perft)
            BenchMark::perft(position, command.parse_perft());

        else if (command == UciCommands::go)
            uci_go(command, position, table, worker);

        else if (command == UciCommands::stop)
            uci_stop(worker);

        else if (command == UciCommands::setoption)
            uci_setoption(command, table);

        else if (command == UciCommands::bench)
        {
            table.reset();
            BenchMark::bench(position, table);
        }
    }
}