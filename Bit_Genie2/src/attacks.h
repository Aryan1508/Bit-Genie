#pragma once
#include "misc.h"
#include "bitboard.h"
#include "Square.h"

namespace Attacks
{
  inline Bitboard knight_attacks(const Square sq)
  {
    return BitMask::knight_attacks[to_int(sq)];
  }


}