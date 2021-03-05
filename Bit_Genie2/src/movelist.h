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

  using iterator = std::array<uint16_t, 256>::iterator;
  using const_iterator = std::array<uint16_t, 256>::const_iterator;
  
  iterator begin()
  {
    return moves.begin();
  }

  iterator end()
  {
    return last;
  }

  int size() const
  {
    return static_cast<int>(last - moves.begin());
  }

  void add(uint16_t&& move)
  {
    *last++ = std::move(move);
  }

private:
  iterator last;
  std::array<uint16_t, 256> moves;
};