#pragma once
#include <sstream>
#include <vector>

enum class UciCommands 
{
  uci,
  quit,
  isready,
  position,

  // *debugging purpose commands*
  print,
  perft,
};

class UciParser
{
public:
  UciParser() = default;
  
  bool operator==(UciCommands) const;
  void take_input();

  std::pair<std::string, std::vector<std::string>> 
    parse_position_command() const;

private:
  void clean_input();
private:
  std::string command;
};