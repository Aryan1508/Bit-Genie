#include "uci.h"
#include "position.h"
#include "search.h"
#include "uciparse.h"
#include "tt.h"
#include "eval.h"
#include "stringparse.h"
#include "benchmark.h"

void uci_input_loop()
{
	std::cout << "Bit Genie by Aryan Parekh" << std::endl;
	UciParser command;
	Position position;
	TTable table(2);

	while (true)
	{
		command.take_input();

		if (command == UciCommands::quit)  break;

		else if (command == UciCommands::isready)
			std::cout << "readyok" << std::endl;

		else if (command == UciCommands::uci)
		{
			std::cout << "id name Bit Genie" << std::endl;
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

			if (options.movetime != -1)
			{
				search.limits.movetime = options.movetime + options.inc;
				search.limits.time_set = true;
			}
			else
				search.limits.movetime = std::numeric_limits<int64_t>::max();

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