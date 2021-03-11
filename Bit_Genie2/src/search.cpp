#include "search.h"
#include "movegen.h"
#include "position.h"
#include "eval.h"
#include "moveorder.h"
#include "tt.h"
#include <sstream>

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
    int      score = MinEval;
    Move best_move = NullMove;

    SearchResult() = default;

    SearchResult(int best_score, Move best = NullMove)
      : score(best_score), best_move(best)
    {}
  };

  int qsearch(Position& position, Search& search, TTable& tt, int alpha, int beta)
  {
    search.info.nodes++;
    search.limits.update();
    search.info.update_seldepth();

    if (search.info.ply >= MaxPly)
      return eval_position(position);

    int stand_pat = eval_position(position);

    if (stand_pat >= beta)
      return beta;

    alpha = std::max(alpha, stand_pat);

    SearchResult result;
    MoveGenerator<true, MoveGenType::noisy> gen(position);

    sort_qmovelist(gen.movelist, position, search, tt);

    for (auto move : gen.movelist)
    {
      position.apply_move(move);
      search.info.ply++;
      int score = -qsearch(position, search, tt, -beta, -alpha);
      search.info.ply--;
      position.revert_move();

      if (search.limits.stopped)
        return 0;

      alpha = std::max(alpha, score);

      if (alpha >= beta)
        return beta;
    }

    return alpha;
  }

  SearchResult negamax(Position& position, Search& search, TTable& tt,
    int depth, int alpha = MinEval, int beta = MaxEval)
  {
    search.info.nodes++;
    search.limits.update();
    search.info.update_seldepth();

    if (depth <= 0)
      return qsearch(position, search, tt, alpha, beta);

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
    sort_movelist(gen.movelist, position, search, tt);

    for (auto move : gen.movelist)
    {
      position.apply_move(move);
      search.info.ply++;

      int score = -negamax(position, search, tt, depth - 1, -beta , -alpha).score;
      search.info.ply--;
      position.revert_move();

      if (search.limits.stopped)
        return 0; 

      if (score > result.score)
      {
        result.best_move = move;
        result.score = score;
      }

      alpha = std::max(alpha, score);

      if (alpha >= beta)
      {
        if (!move_is_capture(position, move))
        {
          search.history.add(position, move, depth);
          search.killers.add(search.info.ply, move);
        }

        return { beta, result.best_move };
      }
    }

    if (original != alpha)
    {
      tt.add(position, result.best_move);
    }

    return { alpha, result.best_move };
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
      o << "cp " << score / get_score(wPawn);
    }
    return o.str();
  }

  std::vector<Move> get_pv(Position& position, TTable& tt, int depth)
  {
    std::vector<Move> pv;
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
    std::printf("info depth %d seldepth %d nodes %llu score %s time %lld pv ",
      depth, search.info.seldepth, search.info.nodes, 
      print_score(result.score).c_str(), search.limits.time_elapsed());

    for (auto m : get_pv(position, tt, depth))
    {
      std::cout << print_move(m) << ' ';
    }

    std::cout << std::endl;
  }
}

void search_position(Position& position, Search& search, TTable& tt)
{
  tt.reset();

  Move best_move = NullMove;
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