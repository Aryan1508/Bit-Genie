#pragma once
#include "misc.h"
#include "bitboard.h"
#include "position.h"
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

  inline Bitboard knight(const Square sq)
  {
    assert(is_ok(sq));
    return BitMask::knight_attacks[to_int(sq)];
  }

  inline Bitboard king(const Square sq)
  {
    assert(is_ok(sq));
    return BitMask::king_attacks[to_int(sq)];
  }

  inline Bitboard bishop(const Square sq, const Bitboard occ)
  {
    assert(is_ok(sq));
    return Bmagic(to_int(sq), occ.to_uint64_t());
  }

  inline Bitboard rook(const Square sq, const Bitboard occ)
  {
    assert(is_ok(sq));
    return Rmagic(to_int(sq), occ.to_uint64_t());
  }

  inline Bitboard queen(const Square sq, const Bitboard occ)
  {
    assert(is_ok(sq));
    return Qmagic(to_int(sq), occ.to_uint64_t());
  }

  inline Bitboard pawn_push(const Square sq, const Piece::Color color, const Bitboard empty)
  {
    if (color == Piece::white)
    {
      const Bitboard single_push = Bitboard(sq).shift<Direction::north>() & empty;
      const Bitboard double_push = single_push.shift<Direction::north>() & empty & BitMask::rank4;
      return single_push | double_push;
    }
    else
    {
      const Bitboard single_push = Bitboard(sq).shift<Direction::south>() & empty;
      const Bitboard double_push = single_push.shift<Direction::south>() & empty & BitMask::rank5;
      return single_push | double_push;
    }
  }

  inline Bitboard pawn_captues(const Square sq, const Piece::Color color, const Bitboard enemy)
  {
    return BitMask::pawn_attacks[to_int(color)][to_int(sq)] & enemy.to_uint64_t();
  }

  inline Bitboard pawn(const Square sq, Position const& position)
  {
    Bitboard enemy = position.enemy_bb();
    Bitboard empty = ~position.total_occupancy();
    Bitboard attacks;

    attacks |= pawn_push(sq, position.player(), empty);
    attacks |= pawn_captues(sq, position.player(), enemy);
    return attacks;
  }
}