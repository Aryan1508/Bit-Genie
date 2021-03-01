#pragma once
#include "misc.h"
#include "zobrist.h"
#include <array>

class PositionHistory {
public:
  PositionHistory();

  // Add the hash of the current position into 
  // the array
  void add(Position const&);

private:
  std::size_t total;
  std::array<ZobristKey, 256> history;
};