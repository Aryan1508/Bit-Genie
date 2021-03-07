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
    return Bmagic(sq, occ);
  }

  inline uint64_t rook(Square sq, uint64_t occ)
  {
    return Rmagic(sq, occ);
  }

  inline uint64_t queen(Square sq, uint64_t occ)
  {
    return Qmagic(sq, occ);
  }

  inline bool square_attacked(Position const& position, Square sq, Color enemy, uint64_t occupancy)
  {
    uint64_t pawns   = position.pieces.get_piece_bb<Pawn>(enemy);
    uint64_t knights = position.pieces.get_piece_bb<Knight>(enemy);
    uint64_t bishops = position.pieces.get_piece_bb<Bishop>(enemy);
    uint64_t rooks   = position.pieces.get_piece_bb<Rook>(enemy);
    uint64_t queens  = position.pieces.get_piece_bb<Queen>(enemy);
    uint64_t kings   = position.pieces.get_piece_bb<King>(enemy);

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
    return square_attacked(position, sq, enemy, position.total_occupancy());
  }
}