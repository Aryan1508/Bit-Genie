#include "move.h"
#include "position.h"
#include <sstream>

bool move_is_capture(Position const& position, uint16_t move)
{
  return position.pieces.get_piece(move_to(move)) != Empty;
}

std::string print_move(uint16_t move)
{
  std::stringstream o;
  o << move_from(move) << move_to(move);
  if (move_flag(move) == MoveFlag::promotion)
  {
    o << move_promoted(move);
  }
  return o.str();
}