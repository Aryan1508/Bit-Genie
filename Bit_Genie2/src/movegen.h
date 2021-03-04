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
    Bitboard occupancy = position.total_occupancy();

    // Generate moves for all normal pieces
    generate_normal_moves(position, Piece::knight, targets, Attacks::knight);
    generate_normal_moves(position, Piece::king  , targets, Attacks::king);
    generate_normal_moves(position, Piece::bishop, targets, Attacks::bishop, occupancy);
    generate_normal_moves(position, Piece::rook  , targets, Attacks::rook  , occupancy);
    generate_normal_moves(position, Piece::queen , targets, Attacks::queen , occupancy);
    generate_normal_moves(position, Piece::pawn  , targets, Attacks::pawn  , position);

    // Seperate function for castles because 
    // they don't follow the same rules 
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

  void add_normal_moves(Square from, Bitboard attacks)
  {
    assert(is_ok(from));
    while (attacks)
    {
      const Square to = attacks.pop_lsb();
      movelist.add(Move(from, to));
    }
  }

  //  All pieces other king-castles, have the same routine for move gen
  //  
  // 1) Iterate through all the pieces of that type on the board
  // 2) Generate a bitboard of attacks for that piece( in namespace Attacks )
  // 3) Iterate through all the bits in those attacks, and add it to the movelist
  // 
  template<typename Callable, typename... Args>
  void generate_normal_moves(Position const& position, Piece::Type type, Bitboard targets, Callable F, Args const&... args)
  {
    Bitboard pieces = position.pieces.get_piece_bb(Piece(type, position.player()));
    while (pieces)
    {
      const Square sq = pieces.pop_lsb();
      const Bitboard attacks = F(sq, args...) & targets;
      add_normal_moves(sq, attacks);
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

      if (rook == Square::A1 || rook == Square::A8)
      {
        movelist.add(Move(king_sq, rook + Direction::east, Move::castle));
      }

      else
      {
        movelist.add(Move(king_sq, rook + Direction::west, Move::castle));
      }
    }
  }
};