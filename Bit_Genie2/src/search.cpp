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
    MateEval = MaxEval - 1,
    MaxPly = 64,
    MinMateScore = MateEval - MaxPly,
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
    search.info.nodes++;
    search.limits.update();
    if (depth <= 0)
      return eval_position(position);

    SearchResult result;
    MoveGenerator gen(position);

    if (gen.movelist.size() == 0)
    {
      if (position.king_in_check())
        return search.info.ply - MateEval;

      else
        return 0;
    }

    for (auto move : gen.movelist)
    {
      position.apply_move(move);
      search.info.ply++;

      int score = -negamax(position, search, depth - 1, -beta, -alpha).score;
      search.info.ply--;
      position.revert_move();

      if (search.limits.stopped)
        return 0;

      if (score >= result.score)
      {
        result.best_move = move;
        result.score = score;
      }

      alpha = std::max(alpha, result.score);

      if (alpha >= beta)
      {
        return result;
      }
    }

    return result;
  }
  
  int mate_distance(int score)
  {
    if (score > 0)
    {
      return (MateEval - score) / 2 + 1;
    }
    else
    {
      return (MateEval + score) / 2 + 1;
    }
  }

  std::string print_score(int score)
  {
    std::stringstream o;
    if (score > MinMateScore || score < -MinMateScore)
    {
      o << "mate " << mate_distance(score);
    }
    else
    {
      o << "score cp " << score / get_score(wPawn);
    }
    return o.str();
  }

  void print_info_string(SearchResult& result, Search& search, int depth)
  {
    std::printf("info depth %d nodes %llu score %s pv %s\n",
      depth, search.info.nodes, print_score(result.score).c_str(), print_move(result.best_move).c_str());
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
    search.info.ply = 0;
    search.info.nodes = 0;
    auto result = negamax(position, search, depth);

    if (search.limits.stopped)
      break;

    print_info_string(result, search, depth);
    best_move = result.best_move;
  }

  std::cout << "bestmove " << print_move(best_move) << std::endl;
}