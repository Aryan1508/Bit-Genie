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
  
  template<bool check = false>
  void add(Position& position, uint16_t&& move)
  {
    if constexpr (check) 
    {
      if (!position.move_is_legal(move))
        return;
    }
    moves[cap++] = std::move(move);
  }

  std::array<uint16_t, 256> moves;
private:
  int cap;
};