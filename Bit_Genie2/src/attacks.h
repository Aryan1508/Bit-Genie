#pragma once
#include "misc.h"
#include "bitboard.h"
#include "Square.h"
#include "magicmoves.hpp"

namespace Attacks
{
  inline void init()
  {
    printf("initializing Magic Moves... ");
    initmagicmoves();
    printf("done.\n");
  }

  inline Bitboard knight_attacks(const Square sq)
  {
    assert(is_ok(sq));
    return BitMask::knight_attacks[to_int(sq)];
  }

  inline Bitboard queen_attacks(const Square sq, const Bitboard occ)
  {
    assert(is_ok(sq));
    return Qmagic(to_int(sq), occ.to_uint64_t());
  }
}