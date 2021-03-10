#include "eval.h"
#include "position.h"
#include "piece.h"
#include "attacks.h"

enum {
  PawnDoubled = S(-10, -10)
};

static bool pawn_doubled(uint64_t friend_pawns, Square sq)
{
  uint64_t file = BitMask::files[sq];
  return is_several(file & friend_pawns);
}

static int evaluate_pawn(Position const& position, Square sq)
{
  int score = 0;

  Color us = color_of(position.pieces.squares[sq]);
  uint64_t friend_pawns = position.pieces.get_piece_bb<Pawn>(us);

  score += pawn_doubled(friend_pawns, sq) * PawnDoubled;

  return score;
}

template<typename Callable>
static int evaluate_piece(Position const& position, Callable F, uint64_t pieces)
{
  int score = 0;
  while (pieces)
  {
    Square sq = pop_lsb(pieces);
    score += F(position, sq);
  }
  return score;
}

template<PieceType type, typename Callable>
static int evaluate_piece(Position const& position, Callable F)
{
  int score = 0;

  uint64_t white = position.pieces.bitboards[type] & position.pieces.colors[White];
  uint64_t black = position.pieces.bitboards[type] & position.pieces.colors[Black];

  score += evaluate_piece(position, F, white);
  score -= evaluate_piece(position, F, black);
  return score;
}  

static int material_balance(uint64_t pieces, PieceType piece)
{
  return popcount64(pieces) * get_score(Piece(piece));
}

static int material_balance(Position const& position)
{
  auto& pieces = position.pieces;
  int score = 0;

  for (int i = 0; i < total_pieces; i++)
  {
    PieceType type = PieceType(i);
    uint64_t white = pieces.bitboards[i] & pieces.colors[White];
    uint64_t black = pieces.bitboards[i] & pieces.colors[Black];

    score += material_balance(white, type);
    score -= material_balance(black, type);
  }
  return score;
}

int eval_position(Position const& position)
{
  int score = 0;

  score += material_balance(position);
  score += evaluate_piece<Pawn>(position, evaluate_pawn);

  return position.side == White ? score : -score;
}