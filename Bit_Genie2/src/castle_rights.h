#pragma once
#include "bitboard.h"
#include <string_view>

// This class uses a bitboard internally 
// to manage castle rights of any position 
class CastleRights 
{
public:
  CastleRights();

  // set to 0 (no castle possible)
  void reset();

  // reset & initialize from a fen-string
  // example: 
  // `kq` (black short, black long)
  // `Kq` (white short, black long)
  // `kQ` (black short, white long)
  // return value is to check validity of fen
  bool set(std::string_view);

  Bitboard data() const { return rooks; }
private:
  // Set a single castle right
  // example:
  // `Q` (white castle long)
  bool set(const char); 

private:
  Bitboard rooks;
  
  // In total a position can have 4 castle possibilites
  // white castle short
  // white castle long
  // black castle short
  // Black castle long
  // 
  // 'rooks' stores the squares of those rooks who
  // can castle. For example, for white to castle
  // short, the rook on H1 is used. 
};

