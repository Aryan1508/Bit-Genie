#pragma once
#include "misc.h"

#define make_score(mg, eg) ((int)((unsigned int)(eg) << 16) + (mg))
#define S(mg, eg) make_score((mg), (eg))

inline int get_score(Piece piece)
{
  constexpr int pawn   = 100;
  constexpr int knight = 300;
  constexpr int bishop = 325;
  constexpr int rook   = 520;
  constexpr int queen  = 950;

  static constexpr int scores[13]
  {
    pawn, knight, bishop, rook, queen, 0
    -pawn, -knight, -bishop, -rook, -queen, 0, 0
  };

  return scores[piece];
}

int eval_position(Position const&);