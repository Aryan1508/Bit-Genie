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
      std::cout << position << '\n';

    else if (command == UciCommands::perft)
    {
      int depth = command.parse_perft();
      if (depth <= 0)
      {
        std::cout << "Invalid perft depth" << std::endl;
        continue;
      }
      uint64_t nodes = position.perft(depth);
      std::cout << "nodes " << nodes << std::endl;
    }
  }
}