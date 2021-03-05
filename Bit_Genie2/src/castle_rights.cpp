#include "castle_rights.h"
#include "move.h"
#include "piece.h"
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

Bitboard CastleRights::get_rooks(Piece::Color color) const
{
  return color == Piece::white ? rooks & BitMask::rank1 : rooks & BitMask::rank8;
}

bool CastleRights::castle_path_is_clear(const Square rook, const Bitboard occupancy)
{
  assert(is_ok(rook));
  static constexpr uint64_t castle_occ_masks[total_squares]{
    0, 0, 0XE, 0, 0, 0X60, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0XE00000000000000, 0, 0, 0, 0X6000000000000000, 0
  };
  return !(castle_occ_masks[to_int(rook)] & occupancy.to_uint64_t());
}

Bitboard CastleRights::get_castle_path(const Square sq)
{
  static constexpr uint64_t castle_paths[total_squares]{
    0, 0, 0X1C, 0, 0, 0, 0X30, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0X1c00000000000000, 0, 0, 0, 0X3000000000000000, 0
  };
  return castle_paths[to_int(sq)];
}


void CastleRights::update(Move move)
{
  static constexpr uint64_t RC1 = 0xfffffffffffffffb;
  static constexpr uint64_t RG1 = 0xffffffffffffffbf;
  static constexpr uint64_t KE1 = 0xffffffffffffffbb;
  static constexpr uint64_t RC8 = 0xfbffffffffffffff;
  static constexpr uint64_t RG8 = 0xbfffffffffffffff;
  static constexpr uint64_t KE8 = 0xbbffffffffffffff;

  static constexpr uint64_t mask[total_squares]{
     RC1, -1, -1, -1, KE1, -1, -1, RG1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    RC8, -1, -1, -1, KE8, -1, -1, RG8,
  };

  rooks &= mask[to_int(move.from())];
  rooks &= mask[to_int(move.to())];
}

bool CastleRights::set(const char right) 
{
  if      (right == 'k') rooks.set(Square::G8);
  else if (right == 'q') rooks.set(Square::C8);
  else if (right == 'K') rooks.set(Square::G1);
  else if (right == 'Q') rooks.set(Square::C1);
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
    return true;

  if (rights.size() == 0)
    return false;

  for (const char right : rights) 
  {
    if (!set(right))  
      return false;
  }
  return true;
}

std::ostream& operator<<(std::ostream& o, const CastleRights rights)
{
  if (rights.rooks.test(Square::G1))  
    o << 'K';
  
  if (rights.rooks.test(Square::C1))  
    o << 'Q';
  
  if (rights.rooks.test(Square::G8))  
    o << 'k';
  
  if (rights.rooks.test(Square::C8))  
    o << 'q';
 
  return o;
}