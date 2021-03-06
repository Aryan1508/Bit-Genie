#pragma once
#include "bitboard.h"
#include "piece.h"
#include <string_view>

// This class uses a uint64_t internally 
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
  bool parse_fen(std::string_view);

  // Return the rooks of the specific color
  uint64_t get_rooks(Piece::Color) const;

  // Some moves disable certain castle rights
  // For example moving the king removes all your castle rights
  // Moving a rook removes that specific castle right
  void update(uint16_t);

  uint64_t data() const { return rooks; }

  static bool castle_path_is_clear(const Square rook, const uint64_t);
  static uint64_t get_castle_path(const Square);

  friend std::ostream& operator<<(std::ostream&, const CastleRights);
private:
  // Set a single castle right
  // example:
  // `Q` (white castle long)
  bool set(const char); 

private:
  uint64_t rooks;
  
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

