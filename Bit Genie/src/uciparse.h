#pragma once
#include <sstream>
#include <vector>

enum class UciCommands
{
	uci,
	quit,
	isready,
	position,
	go,
	stop,
	setoption,


	// *debugging/other purpose commands*
	print,
	perft,
	static_eval,
	printcutoffs,
	bench
};

struct UciGo
{
	int depth = 64;
	int movestogo = -1;

	int64_t btime = -1;
	int64_t wtime = -1;
	int64_t movetime = -1;
};

class UciParser
{
public:
	UciParser() = default;

	bool operator==(UciCommands) const;
	void take_input();

	std::pair<std::string, std::vector<std::string>>
		parse_position_command() const;

	int parse_perft() const;	
	UciGo parse_go() const;
	std::pair<std::string, std::string>
		parse_setoption() const;

public:
	std::string command;
};