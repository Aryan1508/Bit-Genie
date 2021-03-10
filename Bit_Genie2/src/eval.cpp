// Evaluation scores taken from weiss

#include "eval.h"
#include "position.h"
#include "attacks.h"

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

  return position.side == White ? score : -score;
}