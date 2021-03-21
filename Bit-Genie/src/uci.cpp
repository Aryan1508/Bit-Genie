/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie>

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

void uci_input_loop(int argc, char** argv)
{
	std::cout << "Bit Genie by Aryan Parekh" << std::endl;
	UciParser command;
	Position position;
	TTable table(2);

	if (argc > 1 && !strncmp(argv[1], "bench", 5)) {
		BenchMark::bench(position, table);
		return;
	}

	while (true)
	{
		command.take_input();

		if (command == UciCommands::quit)  break;

		else if (command == UciCommands::isready)
			std::cout << "readyok" << std::endl;

		else if (command == UciCommands::uci)
		{
			std::cout << "id name Bit-Genie 1.0.0" << std::endl;
			std::cout << "id author Aryan Parekh" << std::endl;
			std::cout << "option name Hash type spin default 2 min 2 max 3000" << std::endl;
			std::cout << "option name Clear Hash type button" << std::endl;
			std::cout << "uciok" << std::endl;
		}

		else if (command == UciCommands::position)
		{
			auto [fen, moves] = command.parse_position_command();

			if (!position.set_fen(fen))
			{
				std::cout << fen << std::endl;
				std::cout << "Invalid FEN string" << std::endl;
				continue;
			}

			Position temp = position;

			temp.history.total = 0;
			for (std::string move : moves)
			{
				if (!temp.apply_move(move))
				{
					std::cout << "Invalid move in movelist: " << move << std::endl;
					continue;
				}
			}

			position = temp;
		}

		else if (command == UciCommands::print)
			std::cout << position << std::endl;

		else if (command == UciCommands::perft)
		{
			int depth = command.parse_perft();
			if (depth <= 0)
			{
				std::cout << "Invalid perft depth" << std::endl;
				continue;
			}

			BenchMark::perft(position, depth);
		}

		else if (command == UciCommands::go)
		{
			UciGo options = command.parse_go(position.side);

			Search search;
			search.limits.stopwatch.go();
			search.limits.max_depth = std::min(options.depth, 64);
			search.limits.stopped = false;
			search.limits.time_set = false;
			search.limits.time_set = true;

			if (options.movetime == -1)
			{
				auto& t   = position.side == White ? options.wtime : options.btime;
				auto& inc = position.side == White ? options.winc  : options.binc;
			
				if (t == -1)
					search.limits.movetime = std::numeric_limits<int64_t>::max();

				else
					search.limits.movetime = t / options.movestogo + inc - 50;
			}
			else
				search.limits.movetime = options.movetime - 50;

			search_position(position, search, table);
		}

		else if (command == UciCommands::setoption)
		{
			auto [name, value] = command.parse_setoption();

			if (name == "hash")
			{
				if (!string_is_number(value))
					continue;
				table.resize(std::stoi(value));
			}

			else if (name == "clear hash")
			{
				table.reset();
			}
		}

		else if (command == UciCommands::bench)
		{
			BenchMark::bench(position, table);
		}

		else if (command == UciCommands::static_eval)
		{
			std::cout << "eval: " << eval_position(position) << std::endl;
		}

		else if (command == UciCommands::printcutoffs)
		{
			Search search;
			search.limits.movetime = 5000;
			search.limits.max_depth = 64;
			search.limits.time_set = true;
			search.limits.stopwatch.go();
			search.limits.stopped = false;

			search_position(position, search, table);
			print_cutoffs(search);
		}
	}
}