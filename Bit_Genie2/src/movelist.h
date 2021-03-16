#pragma once
#include "misc.h"
#include "move.h"
#include "position.h"
#include <array>
#include <iterator>

class Movelist
{
public:
	using movelist_type  = std::array<Move, 256>;
	using iterator	     = movelist_type::iterator;
	using const_iterator = movelist_type::const_iterator;

	Movelist()
		: cap(0)
	{}

	void clear()
	{
		cap = 0;
	}

	iterator begin()
	{
		return moves.begin();
	}

	const_iterator begin() const
	{
		return moves.begin();
	}

	iterator end()
	{
		return moves.begin() + cap;
	}

	const_iterator end() const
	{
		return moves.begin() + cap;
	}

	int size() const
	{
		return cap;
	}

	Move& operator[](size_t pos) { return moves[pos]; }

	template<bool check = false>
	void add(Position& position, Move&& move)
	{
		if constexpr (check)
		{
			if (!position.move_is_legal(move))
				return;
		}
		moves[cap++] = std::move(move);
	}


private:
	movelist_type moves;
	std::ptrdiff_t cap;
};