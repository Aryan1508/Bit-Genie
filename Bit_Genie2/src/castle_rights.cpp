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
  rooks = 0;
}

uint64_t CastleRights::get_rooks(Color color) const
{
  return color == White ? rooks & BitMask::rank1 : rooks & BitMask::rank8;
}

bool CastleRights::castle_path_is_clear(const Square rook, const uint64_t occupancy)
{
  switch (rook)
  {
  case Square::C1:
    return !(occupancy & 0xE);

  case Square::G1:
    return !(occupancy & 0x60);

  case Square::C8:
    return !(occupancy & 0xe00000000000000);

  case Square::G8:
    return !(occupancy & 0x6000000000000000);

  default:
    assert(false);
    break;
  }
}

uint64_t CastleRights::get_castle_path(Square rook)
{
  switch (rook)
  {
  case Square::C1:
    return 0x8;

  case Square::G1:
    return 0x20;

  case Square::C8:
    return 0x800000000000000;

  case Square::G8:
    return 0x2000000000000000;

  default:
    assert(false);
    break;
  }
}


void CastleRights::update(uint16_t move)
{
  static constexpr uint64_t RC1 = 0xfffffffffffffffb;
  static constexpr uint64_t RG1 = 0xffffffffffffffbf;
  static constexpr uint64_t KE1 = 0xffffffffffffffbb;
  static constexpr uint64_t RC8 = 0xfbffffffffffffff;
  static constexpr uint64_t RG8 = 0xbfffffffffffffff;
  static constexpr uint64_t KE8 = 0xbbffffffffffffff;

  static constexpr uint64_t mask[total_squares]{
     RC1, ~0ull, ~0ull, ~0ull, KE1, ~0ull, ~0ull, RG1,
    ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull,
    ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull,
    ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull,
    ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull,
    ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull,
    ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull, ~0ull,
    RC8, ~0ull, ~0ull, ~0ull, KE8, ~0ull, ~0ull, RG8,
  };

  rooks &= mask[move_from(move)];
  rooks &= mask[move_to(move)];
}

bool CastleRights::set(const char right) 
{
  if      (right == 'k') set_bit(Square::G8, rooks);
  else if (right == 'q') set_bit(Square::C8, rooks);
  else if (right == 'K') set_bit(Square::G1, rooks);
  else if (right == 'Q') set_bit(Square::C1, rooks);
  else return false;

  return true;
}

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
  if (test_bit(Square::G1, rights.rooks))  
    o << 'K';
  
  if (test_bit(Square::C1, rights.rooks))  
    o << 'Q';
  
  if (test_bit(Square::G8, rights.rooks))  
    o << 'k';
  
  if (test_bit(Square::C8, rights.rooks))  
    o << 'q';
 
  return o;
}