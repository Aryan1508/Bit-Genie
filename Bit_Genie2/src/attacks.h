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
    return BitMask::knight_attacks[to_int(sq)];
  }

  inline uint64_t king(Square sq)
  {
    return BitMask::king_attacks[to_int(sq)];
  }

  inline uint64_t bishop(Square sq, uint64_t occ)
  {
    assert(is_ok(sq));
    return Bmagic(to_int(sq), occ);
  }

  inline uint64_t rook(Square sq, uint64_t occ)
  {
    assert(is_ok(sq));
    return Rmagic(to_int(sq), occ);
  }

  inline uint64_t queen(Square sq, uint64_t occ)
  {
    assert(is_ok(sq));
    return Qmagic(to_int(sq), occ);
  }

  inline uint64_t pawn_push(Square sq, const Color color, uint64_t empty)
  {
    if (color == Color::white)
    {
      uint64_t sq_bb = 1ull << to_int(sq);

      uint64_t single_push = shift<Direction::north>(sq_bb) & empty;
      uint64_t double_push = shift<Direction::north>(single_push) & empty & BitMask::rank4;
      return single_push | double_push;
    }
    else
    {
      uint64_t sq_bb = 1ull << to_int(sq);

      uint64_t single_push = shift<Direction::south>(sq_bb) & empty;
      uint64_t double_push = shift<Direction::south>(single_push) & empty & BitMask::rank5;
      return single_push | double_push;
    }
  }

  inline uint64_t pawn_captues(Square sq, const Color color, uint64_t enemy)
  {
    return BitMask::pawn_attacks[to_int(color)][to_int(sq)] & enemy;
  }

  inline uint64_t pawn(Square sq, Position const& position)
  {
    uint64_t enemy = position.enemy_bb();
    uint64_t empty = ~position.total_occupancy();
    uint64_t attacks = 0;

    attacks |= pawn_push(sq, position.player(), empty);
    attacks |= pawn_captues(sq, position.player(), enemy);
    return attacks;
  }

  inline uint64_t pawn_ep(Square sq, Position const& position)
  {
    using Dir = Direction;

    if (position.get_ep() == Square::bad) return 0;

    uint64_t sq_bb = 1ull << to_int(sq);
    uint64_t ep_bb = 1ull << to_int(position.get_ep());

    if (position.player() == Color::white)
    {
      uint64_t ep_rank = BitMask::rank6;

      uint64_t forward = shift<Dir::north>(sq_bb);
      uint64_t left    = shift<Dir::west>(forward);
      uint64_t right   = shift<Dir::east>(forward);

      return (left | right) & ep_bb & ep_rank;
    }
    else
    {
      uint64_t ep_rank = BitMask::rank3;

      uint64_t forward = shift<Dir::south>(sq_bb);
      uint64_t left = shift<Dir::west>(forward);
      uint64_t right = shift<Dir::east>(forward);

      return (left | right) & ep_bb & ep_rank;
    }
  }

  inline bool square_attacked(Position const& position, Square sq, Color enemy)
  {
    auto const& pieces = position.pieces;
    uint64_t us = pieces.get_occupancy(!enemy);
    uint64_t them = pieces.get_occupancy(enemy);
    uint64_t occupancy = us | them;

    uint64_t pawns   = pieces.get_piece_bb<PieceType::pawn>(enemy);
    uint64_t knights = pieces.get_piece_bb<PieceType::knight>(enemy);
    uint64_t bishops = pieces.get_piece_bb<PieceType::bishop>(enemy);
    uint64_t rooks   = pieces.get_piece_bb<PieceType::rook>(enemy);
    uint64_t queens  = pieces.get_piece_bb<PieceType::queen>(enemy);
    uint64_t kings   = pieces.get_piece_bb<PieceType::king>(enemy);

    bishops |= queens;
    rooks |= queens;

    return (BitMask::pawn_attacks[to_int(!enemy)][to_int(sq)] & pawns) 
        || (bishop(sq, occupancy) & bishops)
        || (rook(sq, occupancy)   & rooks)
        || (knight(sq) & knights)
        || (king(sq) & kings);
  }
}