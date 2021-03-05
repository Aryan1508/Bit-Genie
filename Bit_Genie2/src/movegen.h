#pragma once
#include "attacks.h"
#include "bitboard.h"
#include "board.h"
#include "misc.h"
#include "movelist.h" 
#include "position.h"

enum class MoveGenType { normal, noisy, quiet };

template<MoveGenType type>
class MoveGenerator
{
public:
  void generate(Position const& position)
  {
    Bitboard targets = get_targets(position);
    Bitboard occupancy = position.total_occupancy();

    generate_normal_moves(position, Piece::knight, targets, Attacks::knight);
    generate_normal_moves(position, Piece::king  , targets, Attacks::king);
    generate_normal_moves(position, Piece::bishop, targets, Attacks::bishop, occupancy);
    generate_normal_moves(position, Piece::rook  , targets, Attacks::rook  , occupancy);
    generate_normal_moves(position, Piece::queen , targets, Attacks::queen , occupancy);
    
    generate_pawn_moves(position, targets);
    generate_castle(position);
  }

public:
  Movelist movelist;

private:
  Bitboard get_targets(Position const& position)
  {
    //    Normal moves include both noisy and quiet, so all squares
    //    except those occupied by the current player are targets
    //  
    //    Noisy moves are only captures so the targets are 
    //    only squares occupied by the enemy 
    // 
    //    Quiet moves are non-captures so all squares
    //    that areN'T occupied by the current player, nor the enemy
    //

    return type == MoveGenType::normal ? ~position.friend_bb() :
      type == MoveGenType::noisy ? position.enemy_bb() : ~position.total_occupancy();
  }

  template<bool is_promo = false>
  void add_moves(Square from, Bitboard attacks, Move::GenType gen_type)
  {
    assert(is_ok(from));
    while (attacks)
    {
      Square to = attacks.pop_lsb();
      if constexpr (is_promo)
      {
        movelist.add(Move(from, to, gen_type, Move::knight));
        movelist.add(Move(from, to, gen_type, Move::bishop));
        movelist.add(Move(from, to, gen_type, Move::rook));
        movelist.add(Move(from, to, gen_type, Move::queen));

      }
      movelist.add(Move(from, to, gen_type));
    }
  }

  template<typename Callable, typename... Args>
  void generate_normal_moves(Position const& position, Piece::Type p_type, Bitboard targets, Callable F, Args const&... args)
  {
    Bitboard pieces = position.pieces.get_piece_bb(Piece(p_type, position.player()));
    while (pieces)
    {
      Square sq = pieces.pop_lsb();
      Bitboard attacks = F(sq, args...) & targets;
      add_moves(sq, attacks, Move::normal);
    }
  }

  void generate_pawn_moves(Position const& position, Bitboard targets)
  {
    constexpr bool gen_ep = type == MoveGenType::noisy || type == MoveGenType::normal;

    Rank promotion_rank = position.player() == Piece::white ? Rank::seven : Rank::two;

    Bitboard pieces = position.pieces.get_piece_bb(Piece(Piece::pawn, position.player()));

    while (pieces)
    {
      Square sq = pieces.pop_lsb();
      Bitboard attacks = Attacks::pawn(sq, position) & targets;
      
      if (get_square_rank(sq) == promotion_rank)
        add_moves<true>(sq, attacks, Move::promotion);

      else
      {
        add_moves(sq, attacks, Move::normal);

        if constexpr (gen_ep)
          add_moves(sq, Attacks::pawn_ep(sq, position), Move::enpassant);
      }
    }
  }

  bool castle_path_is_attacked(Position const& position, Square rook, Piece::Color enemy)
  {
    Bitboard path = CastleRights::get_castle_path(rook);

    while (path)
    {
      Square sq = path.pop_lsb();
      if (Attacks::square_attacked(position, sq, enemy))
        return true;
    }
    return false;
  }

  void generate_castle(Position const& position)
  {
    Bitboard occupancy = position.total_occupancy();
    bool is_white = position.player() == Piece::white;
    Square king_sq = is_white ? Square::E1 : Square::E8;
    
    Bitboard rooks = position.castle_rights.get_rooks(position.player());
    while (rooks)
    {
      Square rook = rooks.pop_lsb();

      if (!CastleRights::castle_path_is_clear(rook, occupancy))
        continue;

      if (castle_path_is_attacked(position, rook, switch_color(position.player())))
        continue;

      movelist.add(Move(king_sq, rook, Move::castle));
    }
  }
};