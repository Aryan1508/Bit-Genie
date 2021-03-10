#pragma once
#include "misc.h"

#define make_score(mg, eg) ((int)((unsigned int)(eg) << 16) + (mg))
#define S(mg, eg) make_score((mg), (eg))

inline int get_score(Piece piece)
{
  enum
  {
    PawnMG   = 101,  PawnEG   = 160,
    KnightMG = 399,  KnightEG = 486,
    BishopMG = 408,  BishopEG = 499,
    RookMG   = 567,  RookEG   = 875,
    QueenMG  = 1386, QueenEG  = 1588,
  };

  constexpr int pawn   = S(PawnMG, PawnEG);
  constexpr int knight = S(KnightMG, KnightEG);
  constexpr int bishop = S(BishopMG, BishopEG);
  constexpr int rook   = S(RookMG, RookEG);
  constexpr int queen  = S(QueenMG, QueenEG);

  static constexpr int scores[13]
  {
    pawn, knight, bishop, rook, queen, 0
    - pawn, -knight, -bishop, -rook, -queen, 0, 0
  };

  return scores[piece];
}

int eval_position(Position const&);