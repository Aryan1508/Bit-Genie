#pragma once
#include "movegen.h"
#include "tt.h"
#include "killer.h"
#include "position.h"
#include "search.h"


int16_t mvv_lva(Position& position, uint16_t move)
{
  constexpr int scores[]{ 100, 300, 300, 500, 900, 0, 100, 300, 300, 500, 900, 0, 0 };
  Piece victim = position.pieces.squares[move_to(move)];
  Piece attacker = position.pieces.squares[move_from(move)];

  return scores[attacker] - (scores[victim] / 100);
}

int16_t killer_bonus(Search& search, uint16_t move)
{
  int16_t score = 0;

  if (search.killers.first(search.info.ply) == move)
  {
    score += 2000;
  }

  if (search.killers.second(search.info.ply) == move)
  {
    score += 1000;
  }
  
  return score;
}


int16_t evaluate_move(Position& position,
  uint16_t move, TTable& tt, Search& search)
{
  int16_t score = 0;

  if (tt.retrieve(position).move == move)
    score += 10000;

  score += mvv_lva(position, move);
  score += killer_bonus(search, move);

  return score;
}