#pragma once
#include "misc.h"

#define make_score(mg, eg) ((int)((unsigned int)(eg) << 16) + (mg))
#define S(mg, eg) make_score((mg), (eg))

constexpr int get_score(Piece piece)
{
	constexpr int pawn   = S(100, 100);
	constexpr int knight = S(300, 300);
	constexpr int bishop = S(325, 325);
	constexpr int rook   = S(520, 520);
	constexpr int queen  = S(950, 950);

	constexpr int scores[13]
	{
	  pawn, knight, bishop, rook, queen, 0
	  - pawn, -knight, -bishop, -rook, -queen, 0, 0
	};

	return scores[piece];
}

int eval_position(Position const&);