#include "position.h"

Position::Position() 
{
  reset();
}

void Position::reset() 
{
  key.reset();
  pieces.reset();
}

ZobristKey Position::hash() const 
{
  return key;
}

Position::Position(std::string_view fen_string) 
{
  reset();
}