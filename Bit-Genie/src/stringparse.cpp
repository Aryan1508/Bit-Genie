/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stringparse.h"
#include <sstream>
#include <algorithm>
#include <regex>

std::string& ltrim(std::string& s, const char* t)
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

bool string_is_number(std::string_view s)
{
	std::string_view::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

void trim(std::string& str)
{
	if (str.length() == 0)
		return;

	std::stringstream stream(str);
	std::string out;
	std::string temp;

	while (stream >> temp)
		out += temp + ' ';

	out.pop_back();
	str = std::move(out);
}

std::string& tolower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(),
				   [](unsigned char c) { return std::tolower(c); });
	return str;
}

std::vector<std::string> split_string(std::string_view str)
{
	std::vector<std::string> parts;
	std::stringstream stream(str.data());
	std::string temp;
	while (stream >> temp)
	{
		parts.push_back(std::move(temp));
	}
	return parts;
}


std::vector<std::string> split_string(std::string_view str, char delim)
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