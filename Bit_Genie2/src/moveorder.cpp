#include "moveorder.h"
#include "search.h"
#include "movelist.h"
#include <algorithm>

enum
{
  CaptureBonus = 2000,
  FirstKiller = 15000,
  SecondKiller = 10000,
  PrincipleMove = 20000,
};

static constexpr int16_t mvv_lva_scores[13][13]
{
  { 205, 305, 505, 605, 105, 605, 205, 305, 505, 605, 105, 605, 0,  }, // pawn
  { 204, 304, 504, 604, 104, 604, 204, 304, 504, 604, 104, 604, 0,  }, // knight
  { 203, 303, 503, 603, 103, 603, 203, 303, 503, 603, 103, 603, 0,  }, // bishop
  { 202, 302, 502, 602, 102, 602, 202, 302, 502, 602, 102, 602, 0,  }, // rook
  { 201, 301, 501, 601, 101, 601, 201, 301, 501, 601, 101, 601, 0,  }, // queen
  { 204, 304, 504, 604, 104, 604, 204, 304, 504, 604, 104, 604, 0,  }, // king

  { 205, 305, 505, 605, 105, 605, 205, 305, 505, 605, 105, 605, 0,  }, // pawn
  { 204, 304, 504, 604, 104, 604, 204, 304, 504, 604, 104, 604, 0,  }, // knight
  { 203, 303, 503, 603, 103, 603, 203, 303, 503, 603, 103, 603, 0,  }, // bishop
  { 202, 302, 502, 602, 102, 602, 202, 302, 502, 602, 102, 602, 0,  }, // rook
  { 201, 301, 501, 601, 101, 601, 201, 301, 501, 601, 101, 601, 0,  }, // queen
  { 204, 304, 504, 604, 104, 604, 204, 304, 504, 604, 104, 604, 0,  }, // king

  { 0,0,0,0,0,0,0,0,0,0,0,0,0  },
};

static int16_t mvv_lva(Move move, Position& position)
{
  auto& squares = position.pieces.squares;
  return mvv_lva_scores[squares[move_from(move)]][squares[move_to(move)]];
}

static uint16_t history_bonus(Move move, Position& position, Search& search)
{
  return search.history.get(position, move);
}

static int16_t killer_bonus(Move move, Search& search)
{
  int16_t score = 0;

  score += (search.killers.first(search.info.ply) == move) * FirstKiller;
  score += (search.killers.second(search.info.ply) == move) * SecondKiller;
 
  return score;
}

static uint64_t least_valuable_attacker(Position& position,
  uint64_t attackers, Color side, Piece& capturing)
{
  for (int i = 0; i < total_pieces; i++)
  {
    capturing = make_piece(PieceType(i), side);
    uint64_t pieces = position.pieces.bitboards[i] & position.pieces.colors[side] & attackers;

    if (pieces)
      return pieces & ~pieces + 1;
  }
  return 0;
}

static bool lost_material(int16_t scores[16], int index)
{
  return (-scores[index - 1] < 0 && scores[index] < 0);
}

int16_t see(Position& position, Move move)
{
  static constexpr int see_piece_vals[]{
    100, 300, 325, 500, 900, 1000, 
    100, 300, 325, 500, 900, 1000, 0
  };

  int16_t scores[32] = { 0 };
  int index = 0;

  Square from = move_from(move);
  Square to   = move_to(move);
 
  Piece capturing = position.pieces.squares[from];
  Piece captured  = position.pieces.squares[to];
  Color attacker  = color_of(capturing);

  uint64_t from_set = (1ull << from);
  uint64_t occ = position.total_occupancy(), bishops = 0, rooks = 0;

  bishops = rooks = position.pieces.bitboards[Queen];
  bishops |= position.pieces.bitboards[Bishop];
  rooks   |= position.pieces.bitboards[Rook];

  uint64_t attack_def = Attacks::attackers_to_sq(position, to);
  scores[index] = see_piece_vals[captured];

  do 
  {
    index++;
    attacker = !attacker;
    scores[index] = see_piece_vals[capturing] - scores[index - 1];

    if (lost_material(scores, index))
      break;

    attack_def ^= from_set;
    occ        ^= from_set;

    attack_def |= occ & ((Attacks::bishop(to, occ) & bishops) | (Attacks::rook(to, occ) & rooks));
    from_set = least_valuable_attacker(position, attack_def, attacker, capturing);
  } while (from_set);

  while (--index) 
  {
    scores[index - 1] = -(-scores[index - 1] > scores[index] ? -scores[index - 1] : scores[index]);
  }

  return scores[0];
}

template<bool quiescent>
static int16_t evaluate_move(Move move, Position& position, Search& search, TTable& tt)
{
  Move pv = tt.retrieve(position).move;
  Move killer1 = search.killers.first(search.info.ply);
  Move killer2 = search.killers.second(search.info.ply);

  if constexpr (quiescent)
    return see(position, move);

  if (pv == move)
  {
    return PrincipleMove;
  }
  
  if (killer1 == move)
    return FirstKiller;

  if (killer2 == move)
    return SecondKiller;

  if (move_flag(move) == MoveFlag::promotion)
  {
    constexpr int promotion_scores[]{ 0, 6000, 6000, 8000, 9000 };
    return promotion_scores[move_promoted(move)];
  }

  if (move_is_capture(position, move))
    return CaptureBonus + mvv_lva(move, position);
  
  else
    return search.history.get(position, move);
}

template<bool quiescent>
static void evaluate_movelist(Movelist& movelist, Position& position, Search& search, TTable& tt)
{
  for (auto& m : movelist)
  {
    set_move_score(m, evaluate_move<quiescent>(m, position, search, tt));
  }
}

void sort_movelist(Movelist& movelist, Position& position, Search& search, TTable& tt)
{
  evaluate_movelist<false>(movelist, position, search, tt);
  std::sort(movelist.begin(), movelist.end(),
    [](Move l, Move r) { return l > r; });
}

void sort_qmovelist(Movelist& movelist, Position& position, Search& search, TTable& tt)
{
  evaluate_movelist<true>(movelist, position, search, tt);
  std::sort(movelist.begin(), movelist.end(),
    [](Move l, Move r) { return l > r; });
}