#include "moveorder.h"
#include "search.h"
#include "movelist.h"
#include <algorithm>

enum
{
  CaptureBonus = 2000,
  FirstKiller = 3000,
  SecondKiller = 5000,
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

static int16_t killer_bonus(Move move, Search& search)
{
  int16_t score = 0;

  score += (search.killers.first(search.info.ply) == move) * FirstKiller;
  score += (search.killers.second(search.info.ply) == move) * SecondKiller;

  return score;
}

static int16_t evaluate_move(Move move, Position& position, Search& search)
{
  int16_t score = 0;

  score += (move_is_capture(position, move)) * CaptureBonus;
  score += mvv_lva(move, position);
  score += killer_bonus(move, search);

  return score;
}


static void evaluate_movelist(Movelist& movelist, Position& position, Search& search)
{
  for (auto& m : movelist)
  {
    set_move_score(m, evaluate_move(m, position, search));
  }
}

void sort_movelist(Movelist& movelist, Position& position, Search& search)
{
 
  evaluate_movelist(movelist, position, search);
  std::sort(movelist.begin(), movelist.end(),
    [](Move l, Move r) { return l > r; });
}