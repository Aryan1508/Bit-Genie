#include "castle_rights.h"
#include "Square.h"
#include <unordered_map>

CastleRights::CastleRights()
{
  reset();
}

void CastleRights::reset()
{
  rooks.reset();
}

bool CastleRights::set(const char right) 
{
  if      (right == 'k') rooks.set(Square::H8);
  else if (right == 'q') rooks.set(Square::A8);
  else if (right == 'K') rooks.set(Square::H1);
  else if (right == 'Q') rooks.set(Square::A1);
  else return false;

  return true;
}

//  k -> black can castle short
//  q -> black can castle long
//  K -> white can castle short
//  Q -> white can castle long
bool CastleRights::parse_fen(std::string_view rights)
{
  reset();
  if (rights == "-")
  {
    return true;
  }

  if (rights.size() == 0)
  {
    return false;
  }

  for (const char right : rights) 
  {
    if (!set(right)) 
    {
      return false;
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& o, const CastleRights rights)
{
  if (rights.rooks.test(Square::H1))
  {
    o << 'K';
  }
  if (rights.rooks.test(Square::A1))
  {
    o << 'Q';
  }
  if (rights.rooks.test(Square::H8))
  {
    o << 'k';
  }
  if (rights.rooks.test(Square::A8))
  {
    o << 'q';
  }
  return o;
}