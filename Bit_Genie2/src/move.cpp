#include "move.h"
#include "position.h"

bool move_is_capture(Position const& position, uint16_t move)
{
  return position.pieces.get_piece(GetMoveTo(move)) != Piece::empty;
}