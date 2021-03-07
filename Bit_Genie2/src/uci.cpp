#include "uci.h"
#include "position.h"
#include "uciparse.h"

void uci_input_loop()
{
  UciParser command;
  Position position;

  while (true)
  {
    command.take_input();

    if (command == UciCommands::quit)  return;
    
    else if (command == UciCommands::isready)
      std::cout << "readyok\n";
    

    else if (command == UciCommands::uci)
    {
      std::cout << "id name Bit Genie\n";
      std::cout << "id author Aryan Parekh\n";
      std::cout << "uciok\n";
    }

    else if (command == UciCommands::position)
    {
      auto [fen, moves] = command.parse_position_command();

      if (!position.set_fen(fen))
      {
        std::cout << "Invalid FEN string\n";
        continue;
      }

      for (std::string move : moves)
      {
        if (!position.apply_move(move))
        {
          std::cout << "Invalid move in movelist\n";
          continue;
        }
      }
    }

    else if (command == UciCommands::print)
      std::cout << position << '\n';

    else if (command == UciCommands::perft)
    {
      int depth = command.parse_perft();
      if (depth <= 0)
      {
        std::cout << "Invalid perft depth\n";
        continue;
      }
      std::cout << "nodes: " << position.perft(depth) << '\n';
    }
  }
}