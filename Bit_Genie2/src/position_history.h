#pragma once
#include <array>
#include "misc.h"
#include "zobrist.h"

class PositionHistory
{
public:
  PositionHistory();

  // Add the hash of the current position into 
  // the array
  void add(Position const&);

  // Reset total to 0
  void reset();
private:
  std::size_t total;
  std::array<ZobristKey, 256> history;
};