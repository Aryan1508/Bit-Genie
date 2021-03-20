/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie>

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
#pragma once
#include <string>
#include <string_view>
#include <vector>

std::string& ltrim(std::string&, const char* t = " \t\n\r\f\v");
void trim(std::string&);

std::string& tolower(std::string&);

bool string_is_number(std::string_view);

bool starts_with(std::string_view, std::string_view);

std::vector<std::string> split_string(std::string_view);
std::vector<std::string> split_string(std::string_view, char);