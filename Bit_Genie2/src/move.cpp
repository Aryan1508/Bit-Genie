#include "move.h"
#include "position.h"
#include <sstream>

bool move_is_capture(Position const& position, uint16_t move)
{
  return position.pieces.get_piece(GetMoveTo(move)) != Empty;
}

std::string print_move(uint16_t move)
{
  std::stringstream o;
  o << GetMoveFrom(move) << GetMoveTo(move);
  if (GetMoveType(move) == MoveFlag::promotion)
  {
    o << GetMovePromoted(move);
  }
  return o.str();
}