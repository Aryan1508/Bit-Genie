#include "eval.h"
#include "position.h"


static int material_balance(uint64_t pieces, PieceType piece)
{
  return popcount64(pieces) * get_score(Piece(piece));
}


static int material_balance(Position const& position)
{
  int score = 0;

  for (int i = 0; i < total_pieces; i++)
  {
    PieceType type = PieceType(i);
    uint64_t white = position.pieces.get_piece_bb(make_piece(type, White));
    uint64_t black = position.pieces.get_piece_bb(make_piece(type, Black));

    score += material_balance(white, type);
    score -= material_balance(black, type);
  }
  return score;
}

int get_score(Piece piece)
{
  static constexpr int scores[]
  {
    100, 300, 325, 500, 900, 0,
    -100, -300, -325, -500, -900, 0, 0
  };

  return scores[piece];
}

int eval_position(Position const& position)
{
  int score = 0;
  score += material_balance(position);
  return score;
}