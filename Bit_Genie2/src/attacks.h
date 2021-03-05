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

  inline Bitboard pawn_ep(const Square sq, Position const& position)
  {
    using Dir = Direction;

    if (position.get_ep() == Square::bad) return 0;

    Bitboard sq_bb(sq);
    Bitboard ep_bb(position.get_ep());

    if (position.player() == Piece::white)
    {
      Bitboard ep_rank = BitMask::rank6;

      Bitboard forward = sq_bb.shift<Dir::north>();
      Bitboard left = forward.shift<Dir::west>();
      Bitboard right = forward.shift<Dir::east>();

      return (left | right) & ep_bb & ep_rank;
    }
    else
    {
      Bitboard ep_rank = BitMask::rank3;

      Bitboard forward = sq_bb.shift<Dir::south>();
      Bitboard left = forward.shift<Dir::west>();
      Bitboard right = forward.shift<Dir::east>();

      return (left | right) & ep_bb & ep_rank;
    }
  }

  inline bool square_attacked(Position const& position, Square sq, Piece::Color enemy)
  {
    auto const& pieces = position.pieces;
    Bitboard us = pieces.get_occupancy(switch_color(enemy));
    Bitboard them = pieces.get_occupancy(enemy);
    Bitboard occupancy = us | them;

    Bitboard pawns   = pieces.get_piece_bb(Piece(Piece::pawn  , enemy));
    Bitboard knights = pieces.get_piece_bb(Piece(Piece::knight, enemy));
    Bitboard bishops = pieces.get_piece_bb(Piece(Piece::bishop, enemy));
    Bitboard rooks   = pieces.get_piece_bb(Piece(Piece::rook  , enemy));
    Bitboard queens  = pieces.get_piece_bb(Piece(Piece::queen , enemy));
    Bitboard kings   = pieces.get_piece_bb(Piece(Piece::king  , enemy));

    bishops |= queens;
    rooks |= queens;

    return (BitMask::pawn_attacks[switch_color(enemy)][to_int(sq)] & pawns.to_uint64_t()) 
        || (bishop(sq, occupancy) & bishops)
        || (rook(sq, occupancy)   & rooks)
        || (knight(sq) & knights)
        || (king(sq) & kings);
  }
}