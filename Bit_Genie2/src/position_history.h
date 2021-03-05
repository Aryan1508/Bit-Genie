#pragma once
#include <array>
#include "misc.h"
#include "zobrist.h"

class PositionHistory
{
public:
  // All the info that is expensive to re-calculate 
  // while reverting a move 
  struct Undo
  {
    int          half_moves = 0;
    Square       ep_sq = Square::bad;
    CastleRights castle;
    ZobristKey   key;
    Piece        captured;
  };

  PositionHistory();

  void add(Move, Position const&);
  Piece revert(Position&);

  // Reset total to 0
  void reset();
private:
  int total;
  std::array<Undo, 256> history;
};