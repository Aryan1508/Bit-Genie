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

bool CastleRights::set(const char right) {
  if      (right == 'k') rooks.set(Square::H8);
  else if (right == 'q') rooks.set(Square::A8);
  else if (right == 'K') rooks.set(Square::H1);
  else if (right == 'Q') rooks.set(Square::A1);
  else return false;

  return true;
}

bool CastleRights::set(std::string_view rights)
{
  if (rights.size() == 0)
    return false;

  // In case of an invalid  string,
  // the original bitboard  isn't changed
  Bitboard original = rooks;

  for (const char right : rights) 
  {
    if (!set(right)) {
      // Invalid character encountered, reset to original 
      // form and return false
      rooks = original;
      return false;
    }
  }
  return true;
}

//  k -> black can castle short
//  q -> black can castle long
//  K -> white can castle short
//  Q -> white can castle long