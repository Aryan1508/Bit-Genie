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
#pragma once
#include "../stringparse.h"

class CommandLineParser {
public:
    CommandLineParser(int argc, char **argv) {
        options = std::vector<std::string>(argv, argv + argc);
    }

    std::string get_option(std::string_view key) const {
        for (std::size_t i = 0; i < options.size(); i++) {
            if (options[i] == key)
                return options[i + 1];
        }

        return "";
    }

    uint64_t get_option(std::string_view key, uint64_t default_value) const {
        std::string value = get_option(key);
        return value.size() ? std::stoull(value) : default_value;
    }
private:
    std::vector<std::string> options;
};