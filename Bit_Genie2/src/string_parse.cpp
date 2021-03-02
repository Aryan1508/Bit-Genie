#include "string_parse.h"
#include <sstream>

std::vector<std::string> split_string(std::string_view str, const char delim) 
{
  std::vector<std::string> parts;
  std::stringstream stream(str.data());
  std::string temp;

  while (std::getline(stream, temp, delim))
  {
    parts.push_back(std::move(temp));
  }
  return parts;
}