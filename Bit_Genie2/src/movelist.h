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

  uint16_t* begin()
  {
    return moves;
  }

  uint16_t* end()
  {
    return moves + cap;
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
private:
  int cap;
  uint16_t moves[256];
};