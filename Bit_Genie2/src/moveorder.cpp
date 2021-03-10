#include "moveorder.h"
#include "search.h"
#include "movelist.h"
#include <algorithm>

enum
{
  CaptureBonus = 2000,
  FirstKiller = 15000,
  SecondKiller = 10000,
  PrincipleMove = 20000,
};

static int16_t mvv_lva(Move move, Position& position)
{
  static constexpr int scores[]{
    100, 300, 325, 500, 900, 0, 
    100, 300, 325, 500, 900, 0, 0
  };

  Piece victim = position.pieces.squares[move_from(move)];
  Piece attacker = position.pieces.squares[move_to(move)];

  return scores[victim] - scores[attacker] / 100;
}

static uint16_t history_bonus(Move move, Position& position, Search& search)
{
  return search.history.get(position, move);
}

static int16_t killer_bonus(Move move, Search& search)
{
  int16_t score = 0;

  score += (search.killers.first(search.info.ply) == move) * FirstKiller;
  score += (search.killers.second(search.info.ply) == move) * SecondKiller;
 
  return score;
}

static int16_t evaluate_move(Move move, Position& position, Search& search, TTable& tt)
{
  Move pv = tt.retrieve(position).move;
  Move killer1 = search.killers.first(search.info.ply);
  Move killer2 = search.killers.second(search.info.ply);

  if (pv == move)
    return PrincipleMove;

  if (killer1 == move)
    return FirstKiller;

  if (killer2 == move)
    return SecondKiller;

  if (move_flag(move) == MoveFlag::promotion)
  {
    constexpr int promotion_scores[]{ 0, 6000, 6000, 8000, 9000 };
    return promotion_scores[move_promoted(move)];
  }

  if (move_is_capture(position, move))
  {
    return CaptureBonus + mvv_lva(move, position);
  }
  else
  {
    return search.history.get(position, move);
  }
}


static void evaluate_movelist(Movelist& movelist, Position& position, Search& search, TTable& tt)
{
  for (auto& m : movelist)
  {
    set_move_score(m, evaluate_move(m, position, search, tt));
  }
}

void sort_movelist(Movelist& movelist, Position& position, Search& search, TTable& tt)
{
 
  evaluate_movelist(movelist, position, search, tt);
  std::sort(movelist.begin(), movelist.end(),
    [](Move l, Move r) { return l > r; });
}