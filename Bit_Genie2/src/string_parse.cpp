#include "string_parse.h"
#include <sstream>

bool string_is_number(std::string_view s)
{
  std::string_view::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}

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