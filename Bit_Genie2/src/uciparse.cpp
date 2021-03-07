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

int UciParser::parse_perft() const
{
  auto options = split_string(command);

  if (options.size() != 2)
    return 0;

  if (!string_is_number(options[1]))
    return 0;

  return std::stoi(options[1]);
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

  std::stringstream stream(command);
  std::string name;

  while(stream >> name)
  {
    if (name == "startpos")
      fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    else if (name == "fen")
    {
      auto last = command.find("moves");
      if (last == std::string::npos)
      {
        fen = command.substr(command.find("fen ") + 4);
      }
      else
      {
        fen = command.substr(last);
        break;
      }
    }

    else if (name == "moves")
    {
      while (stream >> name)
      {
        moves.push_back(std::move(name));
      }
      break;
    }
  }

  return std::pair{ fen, moves };
}