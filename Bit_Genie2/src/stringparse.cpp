#include "stringparse.h"
#include <sstream>
#include <algorithm>

bool string_is_number(std::string_view s)
{
  std::string_view::const_iterator it = s.begin();
  while (it != s.end() && std::isdigit(*it)) ++it;
  return !s.empty() && it == s.end();
}

std::string remove_extra_whitespaces(std::string_view str)
{
  std::string out;
  std::unique_copy(str.begin(), str.end(), std::back_insert_iterator<std::string>(out),
                   [](char a, char b){ return std::isspace(a) && std::isspace(b); });
  return out;
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

bool starts_with(std::string_view str, std::string_view comp)
{
  return str.rfind(comp, 0) == 0;
}