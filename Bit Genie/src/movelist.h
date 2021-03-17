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

	size_t size() const
	{
		return static_cast<size_t>(cap);
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

	bool find(Move move)
	{
		for (auto m : *this)
		{
			if (move_without_score(m) == move_without_score(move))
				return true;
		}
		return false;
	}

private:
	movelist_type moves;
	std::ptrdiff_t cap;
};