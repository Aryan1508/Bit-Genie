#include "search.h"
#include "movegen.h"
#include "position.h"
#include "eval.h"

std::atomic_bool SEARCH_ABORT_SIGNAL = false;

namespace
{
  enum
  {
    MinEval = -std::numeric_limits<int>::max(),
    MaxEval = -MinEval,
  };

  struct SearchResult
  {
    uint16_t best_move = 0;
    int      score     = MinEval;

    SearchResult() = default;

    SearchResult(int best_score)
      : score(best_score)
    {}
  };

  SearchResult negamax(Position& position, Search& search, int depth, int alpha = MinEval, int beta = MaxEval)
  {
    if (depth <= 0)
      return eval_position(position);

    SearchResult result;
    MoveGenerator gen(position);

    for (auto move : gen.movelist)
    {
      position.apply_move(move);
      search.info.ply++;

      int score = -negamax(position, search, depth - 1, -beta, -alpha).score;

      if (score >= result.score)
      {
        result.best_move = move;
        result.score = score;
      }

      position.revert_move(); 
      search.info.ply--;
    }

    return result;
  }

  void print_info_string(SearchResult& result, Search& search, int depth)
  {
    std::printf("info depth %d nodes %llu score %d pv %s\n",
      depth, search.info.nodes, result.score, print_move(result.best_move).c_str());
    std::fflush(stdout);
  }
}

void search_position(Position& position, Search& search)
{
  uint16_t best_move = 0;
  for (int depth = 1; 
    depth <= search.limits.max_depth;
    depth++)
  {
    auto result = negamax(position, search, depth);
    print_info_string(result, search, depth);
    best_move = result.best_move;
  }

  std::cout << "bestmove " << print_move(best_move) << std::endl;
}