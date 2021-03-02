#include "position.h"

Position::Position() 
{
  reset();
}

void Position::reset_enpassant()
{
  ep_sq = Square::bad;
}

void Position::reset() 
{
  key.reset();
  pieces.reset();
  reset_enpassant();
}

ZobristKey Position::hash() const 
{
  return key;
}

Position::Position(std::string_view fen_string) 
{
  reset();
}