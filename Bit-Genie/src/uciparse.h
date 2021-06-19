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
#include <sstream>
#include <vector>
#include "misc.h"

enum class UciCommands
{
    uci,
    quit,
    isready,
    position,
    go,
    stop,
    setoption,
    ucinewgame,

    // *debugging/other purpose commands*
    print,
    perft,
    bench
};

struct UciGo
{
    int depth = 64;
    int movestogo = 30;

    int64_t btime = -1;
    int64_t wtime = -1;
    int64_t movetime = -1;
    int64_t binc = -1;
    int64_t winc = -1;
};

class UciParser
{
public:
    UciParser() = default;

    bool operator==(UciCommands) const;
    bool take_input();

    std::pair<std::string, std::vector<std::string>>
    parse_position_command() const;

    int parse_perft() const;
    UciGo parse_go(Color) const;
    std::pair<std::string, std::string>
    parse_setoption() const;

public:
    std::string command;
};