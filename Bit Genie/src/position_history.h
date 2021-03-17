#pragma once
#include <array>
#include "misc.h"
#include "move.h"
#include "zobrist.h"

class PositionHistory
{
public:
	struct Undo
	{
		int          half_moves = 0;
		Square       ep_sq = Square::bad_sq;
		CastleRights castle;
		ZobristKey   key;
		Piece        captured;
		Move     move;
	};

	PositionHistory()
	{
		total = 0;
	}

	Undo& current()
	{
		return history[total];
	}

	Undo const& previous() const
	{
		return history[total - 1];
	}

	void reset()
	{
		total = 0;
	}

	bool is_drawn(ZobristKey current) const
	{
		for (int i = 0; i < total; i++)
		{
			if (history[i].key == current)
				return true;
		}
		return false;
	}


	int total = 0;
private:
	std::array<Undo, 2048> history;
};