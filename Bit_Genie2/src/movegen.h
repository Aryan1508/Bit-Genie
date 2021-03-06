#pragma once
#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "misc.h"
#include "move.h"
#include "movelist.h" 
#include "position.h"

enum class MoveGenType : uint8_t { normal, noisy, quiet };

template<MoveGenType type = MoveGenType::normal>
class MoveGenerator
{
public:
  void generate(Position const& position)
  {
    uint64_t targets = get_targets(position);
    uint64_t occupancy = position.total_occupancy();

    generate_normal_moves(position, PieceType::king  , targets, Attacks::king);
    generate_normal_moves(position, PieceType::knight, targets, Attacks::knight);
    generate_normal_moves(position, PieceType::bishop, targets, Attacks::bishop, occupancy);
    generate_normal_moves(position, PieceType::rook  , targets, Attacks::rook  , occupancy);
    generate_normal_moves(position, PieceType::queen , targets, Attacks::queen , occupancy);
    
    generate_pawn_moves(position, targets);
  //  generate_castle(position);
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
  void add_moves(Square from, uint64_t attacks, MoveFlag gen_type)
  {
    assert(is_ok(from));
    while (attacks)
    {
      Square to = pop_lsb(attacks);
      if constexpr (is_promo)
      {
        movelist.add(Move(from, to, gen_type, PieceType::knight));
        movelist.add(Move(from, to, gen_type, PieceType::bishop));
        movelist.add(Move(from, to, gen_type, PieceType::rook));
        movelist.add(Move(from, to, gen_type, PieceType::queen));

      }
      movelist.add(Move(from, to, gen_type, PieceType::knight));
    }
  }

  template<typename Callable, typename... Args>
  void generate_normal_moves(Position const& position, PieceType p_type, uint64_t targets, Callable F, Args const&... args)
  {
    uint64_t pieces = position.pieces.get_piece_bb(make_piece(p_type, position.player()));
    while (pieces)
    {
      Square sq = pop_lsb(pieces);
      uint64_t attacks = F(sq, args...) & targets;
      add_moves(sq, attacks, MoveFlag::normal);
    }
  }

  template<bool is_promo = false>
  inline void add_pawn_moves(uint64_t attacks, Direction delta, MoveFlag gen_type = MoveFlag::normal)
  {
    while (attacks)
    {
      Square sq = pop_lsb(attacks);
      if constexpr (is_promo)
      {
        movelist.add(Move(sq - delta, sq, gen_type, PieceType::knight));
        movelist.add(Move(sq - delta, sq, gen_type, PieceType::bishop));
        movelist.add(Move(sq - delta, sq, gen_type, PieceType::rook));
        movelist.add(Move(sq - delta, sq, gen_type, PieceType::queen));

      }
      else
      {
        movelist.add(Move(sq - delta, sq, gen_type, PieceType::knight));
      }
    }
  }

  void generate_pawn_moves(Position const& position, uint64_t targets)
  {
    constexpr bool gen_ep = type == MoveGenType::noisy || type == MoveGenType::normal;

    uint64_t pawn_st_rank   = position.player() == Color::white ? BitMask::rank4 : BitMask::rank5;
    uint64_t promotion_rank = position.player() == Color::white ? BitMask::rank7 : BitMask::rank2;
    uint64_t ep_rank = position.player() == Color::white ? BitMask::rank6 : BitMask::rank3;

    Direction forward       = position.player() == Color::white ? Direction::north : Direction::south;
    uint64_t empty = ~position.total_occupancy();
    uint64_t enemy = position.enemy_bb();

    uint64_t pawns = position.pieces.get_piece_bb(make_piece(PieceType::pawn, position.player()));

    uint64_t pawns_normal = pawns & ~promotion_rank;
    uint64_t pawns_promo  = pawns & promotion_rank;

    if constexpr (type == MoveGenType::normal || type == MoveGenType::quiet)
    {
      uint64_t push_one_normal = shift(pawns_normal, forward) & empty;
      uint64_t push_two_noraml = shift(push_one_normal, forward) & empty & pawn_st_rank;
      add_pawn_moves(push_one_normal, forward);
      add_pawn_moves(push_two_noraml, forward + forward);
    }

    if constexpr (type == MoveGenType::normal || type == MoveGenType::noisy)
    {
      uint64_t forward_one = shift(pawns_normal, forward);
      uint64_t left = shift<Direction::west>(forward_one) & enemy;
      uint64_t right = shift<Direction::east>(forward_one) & enemy;

      if (position.ep_sq != Square::bad)
      {
        uint64_t ep_bb = 1ull << to_int(position.ep_sq);

        uint64_t left_ep = shift<Direction::west>(forward_one) & ep_bb & ep_rank;
        uint64_t right_ep = shift<Direction::east>(forward_one) & ep_bb & ep_rank;

        add_pawn_moves(left_ep, forward + Direction::west, MoveFlag::enpassant);
        add_pawn_moves(right_ep, forward + Direction::east, MoveFlag::enpassant);
      }

      add_pawn_moves(left, forward + Direction::west);
      add_pawn_moves(right, forward + Direction::east);
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

  void generate_castle(Position const& position)
  {
    uint64_t occupancy = position.total_occupancy();
    bool is_white = position.player() == Color::white;
    Square king_sq = is_white ? Square::E1 : Square::E8;
    
    uint64_t rooks = position.castle_rights.get_rooks(position.player());
    while (rooks)
    {
      Square rook = pop_lsb(rooks);

      if (!(CastleRights::castle_path_is_clear(rook, occupancy)))
        continue;

      if (castle_path_is_attacked(position, rook, !position.player()))
        continue;

      movelist.add(Move(king_sq, rook, MoveFlag::castle, PieceType::knight));
    }
  }
};