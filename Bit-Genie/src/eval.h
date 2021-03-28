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
#include "misc.h"

#define make_score(mg, eg) ((int)((unsigned int)(eg) << 16) + (mg))
#define S(mg, eg) make_score((mg), (eg))

enum {
	PawnScore = 100, KnightScore = 300,
	BishopScore = 325, RookScore = 520,
	QueenScore = 950,
};

constexpr int piece_scores[13]{
	PawnScore, KnightScore, BishopScore, RookScore, QueenScore, 0,
	PawnScore, KnightScore, BishopScore, RookScore, QueenScore, 0,
};

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