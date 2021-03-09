#include "search.h"
#include "movegen.h"
#include "position.h"
#include "eval.h"
#include "tt.h"
#include <sstream>

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

  SearchResult negamax(Position& position, Search& search, TTable& tt,
    int depth, int alpha = MinEval, int beta = MaxEval)
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

    int original = alpha;
    
    for (auto move : gen.movelist)
    {
      position.apply_move(move);
      search.info.ply++;

      int score = -negamax(position, search, tt, depth - 1, -beta, -alpha).score;
      search.info.ply--;
      position.revert_move();

      if (search.limits.stopped)
        return 0;

      if (score >= result.score)
      {
        result.best_move = move;
        result.score = score;
      }

      alpha = std::max(alpha, score);

      if (alpha >= beta)
      {
        tt.add(position, move);
        return result;
      }
    }

    if (original != alpha)
    {
      tt.add(position, result.best_move);
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

  std::vector<uint16_t> get_pv(Position& position, TTable& tt, int depth)
  {
    std::vector<uint16_t> pv;
    int count = 0;

    TEntry* entry = &tt.retrieve(position);

    while (entry->move && depth)
    {
      if (position.move_exists(entry->move))
      {
        position.apply_move(entry->move);
        pv.push_back(entry->move);
        depth--;
        count++;
      }
      else
        break;

      entry = &tt.retrieve(position);
    }

    while (count--)
      position.revert_move();

    return pv;
  }

  void print_info_string(Position& position, SearchResult& result, TTable& tt, Search& search, int depth)
  {
    std::printf("info depth %d nodes %llu score %s pv ",
      depth, search.info.nodes, print_score(result.score).c_str());

    for (auto m : get_pv(position, tt, depth))
    {
      std::cout << print_move(m) << ' ';
    }

    std::cout << std::endl;
  }
}

void search_position(Position& position, Search& search)
{
  static TTable tt(2);

  uint16_t best_move = 0;
  for (int depth = 1; 
    depth <= search.limits.max_depth;
    depth++)
  {
    search.info.ply = 0;
    search.info.nodes = 0;
    auto result = negamax(position, search, tt, depth);

    if (search.limits.stopped)
      break;

    print_info_string(position, result, tt, search, depth);
    best_move = result.best_move;
  }

  std::cout << "bestmove " << print_move(best_move) << std::endl;
}