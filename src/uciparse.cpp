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
#include "uciparse.h"
#include "stringparse.h"
#include <utility>

bool UciParser::take_input() {
    auto &val = std::getline(std::cin, command);
    trim(command);
    return bool(val);
}

int UciParser::parse_perft() const {
    auto options = split_string(command);

    if (options.size() != 2)
        return 0;

    if (!string_is_number(options[1]))
        return 0;

    return std::stoi(options[1]);
}

bool UciParser::operator==(UciCommands type) const {
    switch (type) {
    case UciCommands::uci:
        return command == "uci";

    case UciCommands::isready:
        return command == "isready";

    case UciCommands::position:
        return starts_with(command, "position");

    case UciCommands::quit:
        return command == "quit";

    case UciCommands::print:
        return command == "print";

    case UciCommands::perft:
        return starts_with(command, "perft");

    case UciCommands::stop:
        return command == "stop";

    case UciCommands::go:
        return starts_with(command, "go");

    case UciCommands::setoption:
        return starts_with(command, "setoption");

    case UciCommands::bench:
        return command == "bench";

    case UciCommands::ucinewgame:
        return command == "ucinewgame";

    default:
        return false;
        break;
    }
}

std::pair<std::string, std::vector<std::string>>

UciParser::parse_position_command() const {
    std::string fen;
    std::vector<std::string> moves;

    std::stringstream stream(command);
    std::string name;

    while (stream >> name) {
        if (name == "startpos")
            fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        else if (name == "fen") {
            auto last = command.find("moves");
            if (last == std::string::npos) {
                fen = command.substr(command.find("fen ") + 4);
            } else {
                auto start = command.find("fen ") + 4;
                fen        = command.substr(start, last - start);
                continue;
            }
        }

        else if (name == "moves") {
            while (stream >> name) {
                moves.push_back(std::move(name));
            }
            break;
        }
    }

    return std::pair{fen, moves};
}

UciGo UciParser::parse_go(Color side) const {
    UciGo options;

    auto parts = split_string(command);

    for (auto key = parts.begin(); key != parts.end() - 1; key++) {
        std::string_view value = *(key + 1);

        if (*key == "infinite")
            break;

        if (*key == "depth")
            options.depth = std::stoi(value.data());

        else if (*key == "movetime")
            options.movetime = std::stoi(value.data());

        else if (*key == "movestogo")
            options.movestogo = std::stoi(value.data());

        else if (*key == "btime" && side == CLR_BLACK)
            options.btime = std::stoi(value.data());

        else if (*key == "wtime" && side == CLR_WHITE)
            options.wtime = std::stoi(value.data());

        else if (*key == "winc" && side == CLR_WHITE)
            options.winc = std::stoi(value.data());

        else if (*key == "binc" && side == CLR_BLACK)
            options.binc = std::stoi(value.data());
    }
    return options;
}

std::pair<std::string, std::string> UciParser::parse_setoption() const {
    std::stringstream stream(command);
    std::string name, value;

    for (std::string token; stream >> token;) {
        if (token == "name") {
            stream >> token;

            if (tolower(token) == "clear") {
                name = std::move(token);
                stream >> token;
                name += " " + std::move(tolower(token));
            } else
                name = std::move(token);
        }

        else if (token == "value") {
            stream >> token;
            value = std::move(tolower(token));
            break;
        }
    }
    return {name, value};
}