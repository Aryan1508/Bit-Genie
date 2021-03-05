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
  const auto& pieces = position.pieces;

  history[total].half_moves = position.half_moves;
  history[total].castle     = position.castle_rights;
  history[total].ep_sq      = position.ep_sq;
  history[total].key        = position.key;
  history[total].moving     = pieces.squares[to_int(move.from())];
  
  if (move.type() == Move::enpassant)
  {
    history[total].captured = Piece(Piece::pawn, switch_color(position.side_to_play));
  }
  else
  {
    history[total].captured = pieces.squares[to_int(move.to())];
  }
  total++;
}

PositionHistory::Undo const& PositionHistory::previous() const
{
  return history[total - 1];
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