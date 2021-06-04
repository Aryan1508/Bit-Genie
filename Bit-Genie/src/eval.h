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

enum
{
    PawnScoreMg = 114,
    PawnScoreEg = 216,
    KnightScoreMg = 370,
    KnightScoreEg = 260,
    BishopScoreMg = 396,
    BishopScoreEg = 253,
    RookScoreMg = 596,
    RookScoreEg = 657,
    QueenScoreMg = 1175,
    QueenScoreEg = 1175
};

constexpr int get_score(Piece piece)
{
    constexpr int pawn = S(PawnScoreMg, PawnScoreEg);
    constexpr int knight = S(KnightScoreMg, KnightScoreEg);
    constexpr int bishop = S(BishopScoreMg, BishopScoreEg);
    constexpr int rook = S(RookScoreMg, RookScoreEg);
    constexpr int queen = S(QueenScoreMg, QueenScoreEg);

    constexpr int scores[13]{
        pawn, knight, bishop, rook, queen, 0, -pawn, -knight, -bishop, -rook, -queen, 0, 0};

    return scores[piece];
}

int eval_position(Position const &);
