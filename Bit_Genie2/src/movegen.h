#pragma once
#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "misc.h"
#include "move.h"
#include "movelist.h" 
#include "position.h"

enum class MoveGenType : uint8_t { normal, noisy, quiet };

template<bool checked = true, MoveGenType type = MoveGenType::normal>
class MoveGenerator
{
public:
  void generate(Position& position)
  {
    uint64_t targets = get_targets(position);
    uint64_t occupancy = position.total_occupancy();
    uint64_t king = position.pieces.get_piece_bb<King>(position.side);

    generate_normal_moves(position, King, targets, Attacks::king);
    generate_normal_moves(position, Knight, targets, Attacks::knight);
    generate_normal_moves(position, Bishop, targets, Attacks::bishop, occupancy);
    generate_normal_moves(position, Rook, targets, Attacks::rook, occupancy);
    generate_normal_moves(position, Queen, targets, Attacks::queen, occupancy);

    generate_pawn_moves(position, targets);

    if (!Attacks::square_attacked(position, get_lsb(king), !position.side, occupancy))
      generate_castle(position);
  }

public:
  Movelist movelist;

private:
  uint64_t get_targets(Position const& position)
  {
    //    Normal moves include both noisy and quiet, so all squares
    //    except those occupied by the current player are targets
    //  
    //    Noisy moves are only captures so the targets are 
    //    only squares occupied by the enemy 
    // 
    //    Quiet moves are non-captures so all squares
    //    that areN'T occupied by the current player, nor the enemy
    return type == MoveGenType::normal ? ~position.friend_bb() :
      type == MoveGenType::noisy ? position.enemy_bb() : ~position.total_occupancy();
  }

  template<bool is_promo = false>
  void add_moves(Position& pos, Square from, uint64_t attacks, MoveFlag gen_type)
  {
    assert(is_ok(from));
    while (attacks)
    {
      Square to = pop_lsb(attacks);
      if constexpr (is_promo)
      {
        movelist.add<checked>(pos, Move(from, to, gen_type, Knight));
        movelist.add<checked>(pos, Move(from, to, gen_type, Bishop));
        movelist.add<checked>(pos, Move(from, to, gen_type, Rook));
        movelist.add<checked>(pos, Move(from, to, gen_type, Queen));

      }
      movelist.add<checked>(pos, Move(from, to, gen_type, Knight));
    }
  }

  template<typename Callable, typename... Args>
  void generate_normal_moves(Position& position, PieceType p_type, uint64_t targets, Callable F, Args const&... args)
  {
    uint64_t pieces = position.pieces.get_piece_bb(make_piece(p_type, position.side));
    while (pieces)
    {
      Square sq = pop_lsb(pieces);
      uint64_t attacks = F(sq, args...) & targets;
      add_moves(position, sq, attacks, MoveFlag::normal);
    }
  }

  template<bool is_promo = false>
  inline void add_pawn_moves(Position& pos, uint64_t attacks, Direction delta, MoveFlag gen_type = MoveFlag::normal)
  {
    while (attacks)
    {
      Square sq = pop_lsb(attacks);
      if constexpr (is_promo)
      {
        movelist.add<checked>(pos, Move(sq - delta, sq, gen_type, Knight));
        movelist.add<checked>(pos, Move(sq - delta, sq, gen_type, Bishop));
        movelist.add<checked>(pos, Move(sq - delta, sq, gen_type, Rook));
        movelist.add<checked>(pos, Move(sq - delta, sq, gen_type, Queen));

      }
      else
      {
        movelist.add<checked>(pos, Move(sq - delta, sq, gen_type, Knight));
      }
    }
  }

  void generate_pawn_moves(Position& position, uint64_t targets)
  {
    constexpr bool gen_ep = type == MoveGenType::noisy || type == MoveGenType::normal;

    uint64_t pawn_st_rank = position.side == White ? BitMask::rank4 : BitMask::rank5;
    uint64_t promotion_rank = position.side == White ? BitMask::rank7 : BitMask::rank2;
    uint64_t ep_rank = position.side == White ? BitMask::rank6 : BitMask::rank3;

    Direction forward = position.side == White ? Direction::north : Direction::south;
    uint64_t empty = ~position.total_occupancy();
    uint64_t enemy = position.enemy_bb();

    uint64_t pawns = position.pieces.get_piece_bb(make_piece(Pawn, position.side));

    uint64_t pawns_normal = pawns & ~promotion_rank;
    uint64_t pawns_promo = pawns & promotion_rank;

    if constexpr (type == MoveGenType::normal || type == MoveGenType::quiet)
    {
      uint64_t push_one_normal = shift(pawns_normal, forward) & empty;
      uint64_t push_two_noraml = shift(push_one_normal, forward) & empty & pawn_st_rank;

      add_pawn_moves(position, push_one_normal, forward);
      add_pawn_moves(position, push_two_noraml, forward + forward);

      push_one_normal = shift(pawns_promo, forward) & empty;
      add_pawn_moves<true>(position, push_one_normal, forward, MoveFlag::promotion);
    }

    if constexpr (type == MoveGenType::normal || type == MoveGenType::noisy)
    {
      uint64_t forward_one = shift(pawns_normal, forward);
      uint64_t left = shift<Direction::west>(forward_one) & enemy;
      uint64_t right = shift<Direction::east>(forward_one) & enemy;

      if (position.ep_sq != Square::bad_sq)
      {
        uint64_t ep_bb = 1ull << to_int(position.ep_sq);

        uint64_t left_ep = shift<Direction::west>(forward_one) & ep_bb & ep_rank;
        uint64_t right_ep = shift<Direction::east>(forward_one) & ep_bb & ep_rank;

        add_pawn_moves(position, left_ep, forward + Direction::west, MoveFlag::enpassant);
        add_pawn_moves(position, right_ep, forward + Direction::east, MoveFlag::enpassant);
      }

      add_pawn_moves(position, left, forward + Direction::west);
      add_pawn_moves(position, right, forward + Direction::east);

      forward_one = shift(pawns_promo, forward);
      left = shift<Direction::west>(forward_one) & enemy;
      right = shift<Direction::east>(forward_one) & enemy;

      add_pawn_moves<true>(position, left, forward + Direction::west, MoveFlag::promotion);
      add_pawn_moves<true>(position, right, forward + Direction::east, MoveFlag::promotion);
    }
  }

  bool castle_path_is_attacked(Position const& position, Square rook, Color enemy)
  {
    uint64_t path = CastleRights::get_castle_path(rook);

    while (path)
    {
      Square sq = pop_lsb(path);
      if (Attacks::square_attacked(position, sq, enemy))
        return true;
    }
    return false;
  }

  void generate_castle(Position& position)
  {
    uint64_t occupancy = position.total_occupancy();
    bool is_white = position.side == White;
    Square king_sq = is_white ? Square::E1 : Square::E8;

    uint64_t rooks = position.castle_rights.get_rooks(position.side);
    while (rooks)
    {
      Square rook = pop_lsb(rooks);

      if (!(CastleRights::castle_path_is_clear(rook, occupancy)))
        continue;

      if (castle_path_is_attacked(position, rook, !position.side))
        continue;

      movelist.add<checked>(position, Move(king_sq, rook, MoveFlag::castle, 1));
    }
  }
};