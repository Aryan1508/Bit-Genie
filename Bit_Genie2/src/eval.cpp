#include "eval.h"
#include "position.h"
#include "piece.h"
#include "attacks.h"
#include "board.h"

enum {
  PawnDoubled = S(-10, -10),
  PawnIsolated = S(-15, -15)
};

// evaluation scores are taken from weiss 

static constexpr int pawn_psqt[]
{ 
  S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0),
  S( 50, 69), S( 37, 60), S( 39, 37), S( 33,  9), S( 37,  0), S( 14,  8), S(  3, 24), S(  3, 40),
  S(  5, 76), S( 27, 62), S( 40, 32), S( 33, -4), S( 48, -8), S( 95, 13), S( 47, 34), S( 16, 43),
  S( -2, 34), S( -2, 21), S(  2,  6), S( 24,-26), S( 33,-21), S( 36,-12), S(  4,  7), S( -2,  9),
  S(-14, 12), S(-14,  7), S( -3,-12), S(  0,-18), S(  8,-18), S(  5,-12), S( -5, -6), S(-14, -6),
  S(-18,  2), S(-22, -4), S(-18, -4), S(-12,-10), S( -6, -3), S( -6,  0), S( -1,-14), S( -9,-13),
  S( -7,  7), S(-12,  1), S(-15,  7), S( -6,  3), S(-11, 11), S( 11, 10), S( 15,-10), S(  1,-21),
  S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0), S(  0,  0) 
};

static constexpr int knight_psqt[]
{ 
  S(-60,-67), S(-10,-10), S(-17, -6), S( -2, -7), S(  2, -1), S(-12, -5), S(-16,-16), S(-58,-47),
  S(-19,-19), S(-14, -4), S( 24, -9), S( 33, 19), S( 24, 20), S( 20, -4), S( -4,-10), S(-21,-15),
  S(-19,-15), S( 11,  0), S( 28, 26), S( 42, 21), S( 63, 19), S( 53, 33), S( 34,  4), S(  3, -8),
  S(  0, -6), S(  8,  9), S( 26, 31), S( 23, 41), S( 11, 47), S( 32, 29), S(  7, 21), S(  7,  3),
  S( -2, -2), S( 11,  6), S( 11, 35), S( 16, 40), S( 17, 35), S( 16, 35), S( 32,  8), S( 15, 10),
  S(-31,-31), S(-13,-16), S(-13,  2), S(  2, 20), S(  4, 17), S( -8, -2), S( -6,-12), S(-17,-24),
  S(-33,-19), S(-36,-12), S(-25,-27), S( -9, -6), S(-13, -6), S(-23,-27), S(-31, -6), S( -8, -8),
  S(-53,-57), S(-20,-41), S(-24,-27), S(-19, -7), S(-11, -8), S(-15,-21), S(-21,-23), S(-50,-43)
};

static constexpr int bishop_psqt[]
{ 
  S( -6,  4), S(  1,  0), S( -8,  8), S( -6,  7), S( -4,  7), S(-17, -1), S( -5,  5), S( -2,  0),
  S(-31,  0), S(  0, 14), S( -4, 11), S( -1,  9), S(  3,  9), S(  2, 10), S( -7, 10), S(-19,  1),
  S( -1,  8), S( 15, 20), S( 32, 17), S( 22, 10), S( 32, 15), S( 35, 28), S( 26, 22), S(  8,  9),
  S(-16,  4), S( 22, 16), S( 12, 11), S( 45, 14), S( 30, 23), S( 18, 13), S( 17, 23), S(-12, 11),
  S(-10, -2), S(  1,  2), S(  5, 19), S( 20, 18), S( 21, 16), S(  2, 16), S(  2,  6), S(  2, -3),
  S( -3, -4), S(  8,  7), S(  5, 12), S(  6, 15), S(  6, 15), S( 10,  8), S( 14, -2), S( 13,  4),
  S( 13, -9), S(  9,-19), S(  0, -8), S( -9,  1), S( -7,  3), S( -5,-15), S( 14,-15), S( 12,-20),
  S( 17, -5), S(  5,  2), S(  2, -4), S( -8, -8), S(-10, -3), S(  2,  1), S( -1,  1), S( 15, -7) 
};

static constexpr int rook_psqt[]
{ 
  S( 29, 40), S( 23, 46), S( 22, 46), S( 19, 43), S( 33, 40), S( 18, 50), S( 23, 47), S( 20, 51),
  S(  7, 36), S( -1, 41), S( 25, 37), S( 36, 40), S( 31, 41), S( 33, 28), S( 27, 28), S( 29, 27),
  S(  2, 32), S( 40, 20), S( 29, 30), S( 49, 20), S( 58, 16), S( 50, 27), S( 48, 15), S( 32, 21),
  S( -3, 25), S( 12, 23), S( 20, 26), S( 42, 18), S( 34, 18), S( 32, 15), S( 29, 15), S( 20, 19),
  S(-24,  8), S(-15, 19), S(-20, 20), S(-14, 12), S(-17,  9), S(-15,  9), S(  6, 10), S( -5, -2),
  S(-34,-17), S(-19, -4), S(-31, -8), S(-23,-15), S(-25,-16), S(-26,-15), S(  3,-16), S(-22,-21),
  S(-56,-21), S(-26,-22), S(-20,-16), S(-14,-20), S(-13,-23), S(-16,-34), S(-15,-28), S(-44,-22),
  S(-23,-11), S(-17,-11), S(-17, -8), S( -8,-19), S(-12,-22), S( -9,-14), S( -5,-20), S(-14,-27) 
};

static constexpr int queen_psqt[]
{ 
  S( -2,  5), S( 10, 12), S( 24, 15), S( 21, 33), S( 27, 35), S( 27, 27), S( 10, 10), S( 23, 21),
  S(-14, -4), S(-64, 23), S(-11, 14), S( -5, 23), S(  5, 50), S( 35, 33), S( -9, 10), S(  6, 13),
  S(-15, -2), S( -3,-11), S( -7,  6), S(  8, 14), S( 35, 32), S( 57, 50), S( 64, 45), S( 42, 32),
  S( -9,-13), S( -5, 12), S( -8, -5), S( -7, 28), S(  6, 36), S( 12, 43), S( 35, 42), S( 19, 30),
  S( -9, -7), S( -3, -1), S(-11,  1), S(-18, 30), S(-12, 22), S( -7, 26), S(  5, 18), S( 12, 22),
  S(-17,-19), S(  1,-19), S( -9, -4), S(-12,-19), S( -8,-22), S(-13, -2), S(  5,-22), S( -6, -2),
  S(-18,-19), S( -8,-30), S(  4,-64), S(  0,-34), S(  3,-43), S( -8,-63), S(-13,-47), S(-20,-24),
  S(-21,-27), S(-23,-41), S(-16,-52), S( -8,-59), S(-13,-50), S(-27,-48), S(-22,-27), S(-24,-21) 
};

static constexpr int mobility_scores[4][28] 
{
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

template<PieceType type, typename Callable, typename... Args>
static constexpr int mobility_score(Callable F, Args... args)
{
  return mobility_scores[type - 1][popcount64(F(args...))];
}

static constexpr int passed_pawn_scores[total_ranks] = {
    S(0,  0) , S(-16, 22), S(-16, 25), S(-7, 56),
    S(26, 80), S(60,139) , S(136,196), S(0,  0),
};

static bool pawn_doubled(uint64_t friend_pawns, Square sq)
{
  uint64_t file = BitMask::files[sq];
  return is_several(file & friend_pawns);
}

static bool pawn_passed(uint64_t enemy_pawns, Color us, Square sq)
{
  return !(enemy_pawns & BitMask::passed_pawn[us][sq]);
}

static bool pawn_isolated(uint64_t friend_pawns, Square sq)
{
  return !(friend_pawns & BitMask::neighbor_files[sq]);
}

static inline Square psqt_sq(Square sq, Color color)
{
  return color == White ? flip_square(sq) : sq;
}

static int evaluate_pawn(Position const& position, Square sq, Color us)
{
  int score = 0;
  uint64_t friend_pawns = position.pieces.get_piece_bb<Pawn>(us);
  uint64_t enemy_pawns  = position.pieces.get_piece_bb<Pawn>(!us);

  score += pawn_doubled(friend_pawns, sq)   * PawnDoubled;
  score += pawn_isolated(friend_pawns, sq)  * PawnIsolated;
  score += pawn_passed(enemy_pawns, us, sq) * passed_pawn_scores[to_int(rank_of(sq, us))];
  score += -pawn_psqt[psqt_sq(sq, us)];

  return score;
}

static int evaluate_knight(Position const& position, Square sq, Color us)
{
  int score = 0;

  score += knight_psqt[psqt_sq(sq, us)];
  score += mobility_score<Knight>(Attacks::knight, sq);

  return score;
}

static int evaluate_rook(Position const& position, Square sq, Color us)
{
  int score = 0;

  score += rook_psqt[psqt_sq(sq, us)];
  score += mobility_score<Rook>(Attacks::rook, sq, position.total_occupancy());

  return score;
}

static int evaluate_queen(Position const& position, Square sq, Color us)
{
  int score = 0;

  score += queen_psqt[psqt_sq(sq, us)];
  score += mobility_score<Queen>(Attacks::queen, sq, position.total_occupancy());
  return score;
}

static int evaluate_bishop(Position const& position, Square sq, Color us)
{
  int score = 0;

  score += bishop_psqt[psqt_sq(sq, us)];
  score += mobility_score<Bishop>(Attacks::bishop, sq, position.total_occupancy());

  return score;
}

template<typename Callable>
static int evaluate_piece(Position const& position, Callable F, uint64_t pieces, Color us)
{
  int score = 0;
  while (pieces)
  {
    Square sq = pop_lsb(pieces);
    score += F(position, sq, us);
  }
  return score;
}

template<PieceType type, typename Callable>
static int evaluate_piece(Position const& position, Callable F)
{
  int score = 0;

  uint64_t white = position.pieces.bitboards[type] & position.pieces.colors[White];
  uint64_t black = position.pieces.bitboards[type] & position.pieces.colors[Black];

  score += evaluate_piece(position, F, white, Color::White);
  score -= evaluate_piece(position, F, black, Color::Black);
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
  score += evaluate_piece<Knight>(position, evaluate_knight);
  score += evaluate_piece<Rook>(position, evaluate_rook);
  score += evaluate_piece<Bishop>(position, evaluate_bishop);
  score += evaluate_piece<Queen>(position, evaluate_queen);

  return position.side == White ? score : -score;
}