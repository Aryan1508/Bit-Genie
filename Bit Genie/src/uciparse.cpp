#include "uciparse.h"
#include "stringparse.h"
#include <vector>
#include <iostream>
#include "piece.h"

void UciParser::take_input()
{
	std::getline(std::cin, command);
	trim(command);
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

		case UciCommands::static_eval:
			return command == "eval";

		case UciCommands::printcutoffs:
			return command == "cutoff";

		case UciCommands::setoption:
			return starts_with(command, "setoption");

		case UciCommands::bench:
			return command == "bench";

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

	while (stream >> name)
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
				fen = command.substr(command.find("fen ") + 4, last);
				continue;
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

UciGo UciParser::parse_go(Color side) const
{
	UciGo options;

	auto parts = split_string(command);

	for (auto key = parts.begin(); key != parts.end() - 1; key++)
	{
		std::string_view value = *(key + 1);

		if (*key == "infinite")
			break;

		if (*key == "depth")
			options.depth = std::stoi(value.data());

		else if (*key == "movetime")
			options.movetime = std::stoi(value.data());

		else if (*key == "btime" && side == Black)
			options.movetime = std::stoi(value.data());

		else if (*key == "wtime" && side == White)
			options.movetime = std::stoi(value.data());

		else if (*key == "winc" && side == White)
			options.inc = std::stoi(value.data());

		else if (*key == "binc" && side == Black)
			options.inc = std::stoi(value.data());

	}
	return options;
}

std::pair<std::string, std::string>
	UciParser::parse_setoption() const
{
	std::stringstream stream(command);
	std::string name, value;

	for (std::string token; stream >> token;)
	{
		if (token == "name")
		{
			stream >> token;

			if (tolower(token) == "clear")
			{
				name = std::move(token);
				stream >> token;
				name += " " + std::move(tolower(token));
			}
			else
				name = std::move(token);
		}

		if (token == "value")
		{
			stream >> token;
			value = std::move(tolower(token));
			break;
		}
	}
	return { name, value };
}