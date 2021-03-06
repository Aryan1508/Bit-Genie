#pragma once
#include "misc.h"
#include "move.h"
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

  void add(uint16_t&& move)
  {
    moves[cap++] = std::move(move);
  }
private:
  int cap;
  uint16_t moves[256];
};