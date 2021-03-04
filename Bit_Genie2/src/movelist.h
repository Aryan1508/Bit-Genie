#pragma once
#include "misc.h"
#include "move.h"
#include <array>

class Movelist
{
public:
  Movelist()
  {
    last = moves.begin();
  }

  using iterator = std::array<Move, 256>::iterator;
  using const_iterator = std::array<Move, 256>::const_iterator;
  
  iterator begin()
  {
    return moves.begin();
  }

  iterator end()
  {
    return last;
  }

  void add(Move&& move)
  {
    *last++ = std::move(move);
  }

private:
  iterator last;
  std::array<Move, 256> moves;
};