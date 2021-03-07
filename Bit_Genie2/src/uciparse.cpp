#include "uciparse.h"
#include "stringparse.h"
#include <vector>
#include <iostream>

void UciParser::take_input()
{
  std::getline(std::cin, command);
  clean_input();
}

void UciParser::clean_input()
{
  command = remove_extra_whitespaces(command);
}

bool UciParser::operator==(UciCommands type) const
{
  switch (type)
  {
  case UciCommands::uci:
    return command == "uci";
    break;

  case UciCommands::isready:
    return command == "isready";
    break;

  case UciCommands::position:
    return starts_with(command, "position");
    break;

  case UciCommands::quit:
    return command == "quit";
    break;

  case UciCommands::print:
    return command == "print";
    break;

  case UciCommands::perft:
    return starts_with(command, "perft");
    break;

  default:
    return false;
    break;
  }
}

std::pair<std::string, std::vector<std::string>> 
UciParser::parse_position_command() const
{
  std::string fen;
  std::vector<std::string> moves;

  auto options = split_string(command);
  
  for (auto it = options.begin(); it != options.end(); it++)
  {
    if (std::next(it) == options.end())
      break;
    
    std::string_view name = *it;
    std::string_view value = *(it + 1);

    if (value == "startpos")
      fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    else if (name == "fen")
      fen = value;
    
    else if (name == "moves")
    {
      if (fen.size() == 0)
        break;

      moves = std::vector<std::string>(it + 1, options.end());
    }
  }

  return std::pair{ fen, moves };
}