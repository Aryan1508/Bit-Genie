// Evaluation scores taken from weiss

#include "eval.h"
#include "position.h"
#include "attacks.h"

template<PieceType type, bool safe = false>
static int mobility_score(uint64_t attacks)
{
  constexpr int scores[4][28] = {
    { S(-58,-54), S(-24,-67), S( -4,-23), S(  6, 14), S( 15, 29), S( 18, 49), S( 26, 52), S( 36, 48),
      S( 51, 28) },
   
    { S(-55,-95), S(-19,-92), S( -1,-37), S(  6, -4), S( 14, 13), S( 21, 37), S( 25, 53), S( 23, 61),
      S( 22, 69), S( 27, 69), S( 31, 66), S( 58, 52), S( 59, 68), S( 49, 51) },
   
    { S(-59,-69), S(-28,-58), S(-13,-35), S(-12,-17), S( -5, 15), S( -3, 35), S( -3, 54), S(  2, 57),
      S(  9, 61), S( 17, 66), S( 26, 70), S( 28, 71), S( 29, 73), S( 43, 61), S( 88, 35) },
    
    { S(-62,-48), S(-70,-36), S(-66,-49), S(-45,-50), S(-27,-46), S( -9,-45), S(  2,-37), S(  9,-23),
      S( 14, -7), S( 19,  9), S( 21, 24), S( 24, 34), S( 28, 39), S( 27, 49), S( 29, 55), S( 28, 63),
      S( 26, 69), S( 26, 69), S( 24, 73), S( 29, 72), S( 34, 74), S( 51, 63), S( 60, 69), S( 79, 66),
      S(106, 85), S(112, 84), S(104,111), S(108,131) }
  };

  return scores[type - 1][popcount64(attacks)];
}

static int evaluate_bishop(Position const& position, Square sq)
{
  return mobility_score<Bishop>(Attacks::bishop(sq, position.total_occupancy()));
}

static int evaluate_knight(Position const& position, Square sq)
{
  return mobility_score<Knight>(Attacks::knight(sq));
}

template<typename Callable>
static int evaluate_piece(Position const& position, uint64_t pieces, Callable F)
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
  uint64_t white = position.pieces.get_piece_bb<type>(White);
  uint64_t black = position.pieces.get_piece_bb<type>(Black);

  score += evaluate_piece(position, white, F);
  score -= evaluate_piece(position, black, F);

  return score;
}

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

int eval_position(Position const& position)
{
  int score = 0;
  score += material_balance(position);

  score += evaluate_piece<Bishop>(position, evaluate_bishop);


  return position.side == White ? score : -score;
}