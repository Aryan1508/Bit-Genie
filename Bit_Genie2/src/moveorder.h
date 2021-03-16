#pragma once
#include "movegen.h"
#include "tt.h"
#include "killer.h"
#include "position.h"

class MovePicker
{
	enum class Stage
	{
		HashMove, 
		GenNoisy, GiveNoisy,
		Killer1, Killer2, GenQuiet, GiveQuiet
	};

public:
	MovePicker(Position&, Search&, TTable&);
	
	bool next(Move&);

	MoveGenerator<true> gen;
private:
	Position* position;
	Search*   search;
	TTable*   table;

	Stage stage = Stage::HashMove;
	Movelist::iterator current;
};