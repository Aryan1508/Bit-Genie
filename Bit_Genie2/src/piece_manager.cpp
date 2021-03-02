#include "piece_manager.h"
#include "Square.h"

PieceManager::PieceManager() 
{
  reset();
}

void PieceManager::reset()
{
  colors.fill(0);
  pieces.fill(0);
  squares.fill(Piece());
  ep_sq = Square::bad;
}