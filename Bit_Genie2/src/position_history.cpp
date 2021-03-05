#include "move.h"
#include "position.h"
#include "position_history.h"

PositionHistory::PositionHistory() 
  : total(0)
{
  history.fill(Undo());
}

void PositionHistory::reset() 
{
  total = 0;
}

void PositionHistory::add(Move move, Position const& position) 
{
  history[total++] =
  { position.get_halfmoves(), 
    position.get_ep(),
    position.castle_rights, 
    position.key,
    position.pieces.get_piece(move.to())
  };
}

Piece PositionHistory::revert(Position& position)
{
  Undo& previous = history[--total];

  position.half_moves = previous.half_moves;
  position.castle_rights = previous.castle;
  position.ep_sq = previous.ep_sq;
  position.key = previous.key;

  return previous.captured;
}