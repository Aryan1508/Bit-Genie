#pragma once
#include "attacks.h"
#include "bitboard.h"
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

    add_knight_moves(position, targets);
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

  void add_normal_moves(const Square from, Bitboard attacks)
  {
    assert(is_ok(from));
    while (attacks)
    {
      const Square to = attacks.pop_lsb();
      movelist.add(Move(from, to));
    }
  }

  void add_knight_moves(Position const& position, Bitboard knights, Bitboard targets)
  {
    // Pinned knights cannot move, so remove those
    knights &= ~position.get_pinned_mask();

    while (knights)
    {
      const Square sq = knights.pop_lsb();
      add_normal_moves(sq, Attacks::knight_attacks(sq) & targets);
    }
  }

  void add_knight_moves(Position const& position, Bitboard targets)
  {
    const Piece piece = Piece(Piece::knight, position.player());
    const Bitboard knights = position.pieces.get_piece_bb(piece);
    add_knight_moves(position, knights, targets);
  }
};