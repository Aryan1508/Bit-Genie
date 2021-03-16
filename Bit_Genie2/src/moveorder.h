#pragma once
#include "movegen.h"
#include "tt.h"
#include "killer.h"
#include "position.h"

class MovePicker
{
	enum class Stage
	{
		HashMove, Killer1, Killer2,
		GenNoisy, 
	};

public:
	MovePicker(Position&, Search&, TTable&, bool quiescent = false);
	
	bool next(Move&);

private:
	Position* position;
	Search*   search;
	TTable*   table;

	Stage stage = Stage::HashMove;
	MoveGenerator<true> gen;
	bool  is_quiescent;
};