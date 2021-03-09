#pragma once
#include "misc.h"
#include "move.h"
#include "position.h"
#include <array>
#include <iterator>

class Movelist
{
public:
  Movelist()
  {
    cap = 0;
  }

  auto begin()
  {
    return moves.begin();
  }

  auto end()
  {
    return moves.begin() + cap;
  }

  int size() const
  {
    return cap;
  }

  Move& operator[](size_t pos) { return moves[pos]; }
  
  template<bool check = false>
  void add(Position& position, Move&& move)
  {
    if constexpr (check) 
    {
      if (!position.move_is_legal(move))
        return;
    }
    moves[cap++] = std::move(move);
  }

  std::array<Move, 256> moves;
private:
  int cap;
};