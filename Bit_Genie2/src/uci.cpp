#include "uci.h"
#include "position.h"
#include "search.h"
#include "uciparse.h"
#include "tt.h"
#include <thread>
#include <chrono>

static void benchmark_perft(Position position, int depth)
{
  using namespace std::chrono;
  uint64_t nodes = 0;

  auto start = high_resolution_clock::now();
  position.perft(depth, nodes);
  auto stop = high_resolution_clock::now();
  auto time_taken = duration_cast<milliseconds>(stop - start);

  std::cout << "nodes: " << nodes << std::endl;
  std::cout << "time : " << time_taken.count() << " ms" << std::endl;
}

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

      for (std::string move : moves)
      {
        if (!position.apply_move(move))
        {
          std::cout << "Invalid move in movelist" << std::endl;
          continue;
        }
      }
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

      benchmark_perft(position, depth);
    }

    else if (command == UciCommands::go)
    {
      UciGo options = command.parse_go();

      Search search;
      search.limits.max_depth = std::min(options.depth, 64);
      search.limits.start_time = current_time();

      if (options.movetime != -1)
      {
        search.limits.time_set = true;
        search.limits.stop_time = search.limits.start_time + options.movetime;
      } 

      search_position(position, search, table);
    }
  }
}