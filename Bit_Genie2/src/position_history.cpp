#include "position.h"
#include "position_history.h"

PositionHistory::PositionHistory() 
  : total(0)
{
  history.fill(ZobristKey());
}

void PositionHistory::reset() 
{
  total = 0;
}

void PositionHistory::add(Position const& position) 
{
  history[total++] = position.key;
}