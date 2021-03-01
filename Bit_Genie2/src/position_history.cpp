#include "position_history.h"
#include "position.h"

PositionHistory::PositionHistory() 
  : total(0)
{
  history.fill(ZobristKey());
}

void PositionHistory::add(Position const& position) {
  history[total++] = position.hash();
}