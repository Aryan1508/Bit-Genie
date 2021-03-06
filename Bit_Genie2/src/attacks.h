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
    initmagicmoves();
  }

  inline uint64_t knight(Square sq)
  {
    return BitMask::knight_attacks[sq];
  }

  inline uint64_t king(Square sq)
  {
    return BitMask::king_attacks[sq];
  }

  inline uint64_t bishop(Square sq, uint64_t occ)
  {
    assert(is_ok(sq));
    return Bmagic(sq, occ);
  }

  inline uint64_t rook(Square sq, uint64_t occ)
  {
    assert(is_ok(sq));
    return Rmagic(sq, occ);
  }

  inline uint64_t queen(Square sq, uint64_t occ)
  {
    assert(is_ok(sq));
    return Qmagic(sq, occ);
  }

  inline bool square_attacked(Position const& position, Square sq, Color enemy, uint64_t occupancy)
  {
    auto const& pieces = position.pieces;
    uint64_t us   = pieces.get_occupancy(!enemy);
    uint64_t them = pieces.get_occupancy(enemy);

    uint64_t pawns   = pieces.get_piece_bb<Pawn>(enemy);
    uint64_t knights = pieces.get_piece_bb<Knight>(enemy);
    uint64_t bishops = pieces.get_piece_bb<Bishop>(enemy);
    uint64_t rooks   = pieces.get_piece_bb<Rook>(enemy);
    uint64_t queens  = pieces.get_piece_bb<Queen>(enemy);
    uint64_t kings   = pieces.get_piece_bb<King>(enemy);

    bishops |= queens;
    rooks   |= queens;

    return (BitMask::pawn_attacks[to_int(!enemy)][sq] & pawns)
      || (bishop(sq, occupancy) & bishops)
      || (rook(sq, occupancy) & rooks)
      || (knight(sq) & knights)
      || (king(sq) & kings);
  }

  inline bool square_attacked(Position const& position, Square sq, Color enemy)
  {
    auto const& pieces = position.pieces;
    uint64_t us = pieces.get_occupancy(!enemy);
    uint64_t them = pieces.get_occupancy(enemy);
    uint64_t occupancy = us | them;
    
    return square_attacked(position, sq, enemy, occupancy);
  }
}