/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "eval.h"
#include "position.h"
#include "piece.h"
#include "attacks.h"
#include "board.h"

// evaluation scores are taken from weiss - Terje Kirstihagen
static constexpr int open_file_scores[2] = {S(28, 10), S(-9, 5)};
static constexpr int semiopen_file_scores[2] = {S(9, 15), S(1, 5)};

static constexpr int pawn_psqt[]
{ 
    S( 1, 0 ), S( 0, 0 ), S( 0, 0 ), S( 0, 1 ), S( -1, 0 ), S( 0, -1 ), S( 1, 0 ), S( 0, 0 ),
    S( -7, 12 ), S( -8, 15 ), S( -51, -16 ), S( -70, -28 ), S( -42, -12 ), S( -45, -8 ), S( -42, 11 ), S( -30, 38 ),
    S( 8, 52 ), S( 8, 32 ), S( -15, 10 ), S( -50, -20 ), S( -37, -51 ), S( 2, -39 ), S( 2, 13 ), S( -4, 28 ), 
    S( 3, 60 ), S( 10, 22 ), S( -8, 25 ), S( -2, -55 ), S( 1, -27 ), S( -3, 4 ), S( 12, 30 ), S( -11, 73 ),
    S( -10, 78 ), S( -1, 54 ), S( -11, 7 ), S( -1, -51 ), S( -2, -4 ), S( -7, -9 ), S( -1, 63 ), S( -23, 77 ),
    S( -14, -8 ), S( -3, 35 ), S( -14, 2 ), S( -12, 10 ), S( -5, 4 ), S( -12, 39 ), S( 13, -42 ), S( -17, -21 ),
    S( -15, 27 ), S( 2, -24 ), S( -22, 54 ), S( -17, 17 ), S( -14, 52 ), S( 7, -45 ), S( 18, -59 ), S( -18, -52 ), 
    S( 10, 5 ), S( 2, 0 ), S( 0, 0 ), S( 0, 0 ), S( 1, 1 ), S( -1, 0 ), S( 0, 0 ), S( 0, -1 ),
};

static constexpr int knight_psqt[]
{ 
    S( -94, -82 ), S( -37, -36 ), S( -5, -7 ), S( -21, -19 ), S( -6, -2 ), S( -46, -45 ), S( -46, -47 ), S( -101, -101 ),
    S( -22, -10 ), S( -16, 7 ), S( 9, -1 ), S( 5, 10 ), S( 4, -4 ), S( -4, -5 ), S( -2, -4 ), S( -40, -25 ), 
    S( -26, -20 ), S( 0, 0 ), S( 8, 0 ), S( 18, 17 ), S( 23, 22 ), S( 33, 23 ), S( 12, 11 ), S( -2, 0 ),
    S( -5, 1 ), S( 3, 4 ), S( 10, 10 ), S( 35, 32 ), S( 25, 23 ), S( 29, 21 ), S( 10, 9 ), S( 15, 11 ),
    S( -12, -6 ), S( -8, -6 ), S( 5, 5 ), S( 12, 18 ), S( 14, 12 ), S( 13, 12 ), S( 14, 14 ), S( 8, 3 ),
    S( -22, -17 ), S( -15, -14 ), S( -9, -12 ), S( 1, 1 ), S( 8, 6 ), S( -4, -4 ), S( 1, 0 ), S( -22, -21 ),
    S( -11, -12 ), S( -17, -9 ), S( -26, -14 ), S( -1, -13 ), S( -7, -7 ), S( -7, -21 ), S( -17, -10 ), S( -2, -16 ),
    S( -34, -34 ), S( -18, -14 ), S( -31, -23 ), S( -23, -15 ), S( -11, -8 ), S( -22, -20 ), S( -27, -28 ), S( -18, -11 ), 
};

static constexpr int bishop_psqt[]
{ 
    S( -20, -10 ), S( -13, -12 ), S( -30, -17 ), S( -14, -2 ), S( -17, -14 ), S( -12, -7 ), S( -4, -1 ), S( -19, -14 ),
    S( -17, -15 ), S( -1, 1 ), S( -6, -2 ), S( -25, -21 ), S( -1, -3 ), S( 11, 4 ), S( 4, 0 ), S( -30, -19 ),
    S( -14, 3 ), S( 4, 3 ), S( 7, 2 ), S( 9, 2 ), S( 6, 0 ), S( 19, 17 ), S( 12, 5 ), S( 1, 5 ), 
    S( -1, 2 ), S( -2, 0 ), S( 9, 8 ), S( 21, 22 ), S( 24, 22 ), S( 16, 15 ), S( 3, -7 ), S( 4, 1 ),
    S( -3, 1 ), S( 1, 1 ), S( 5, 5 ), S( 18, 16 ), S( 12, 10 ), S( 2, 9 ), S( -6, -8 ), S( 10, 5 ),
    S( 0, 3 ), S( 8, 8 ), S( 8, 9 ), S( 5, 7 ), S( 6, 5 ), S( 10, 8 ), S( 2, 2 ), S( 0, 0 ),
    S( 10, 5 ), S( 8, -3 ), S( 5, 5 ), S( -2, -1 ), S( 6, 3 ), S( 2, -2 ), S( 24, 20 ), S( 2, -1 ), 
    S( -11, -11 ), S( 6, 3 ), S( 7, 7 ), S( -4, 0 ), S( -1, 0 ), S( -9, 4 ), S( -14, -7 ), S( -2, 4 ),
};

static constexpr int rook_psqt[]
{ 
    S( 15, 15 ), S( 12, 10 ), S( 11, 8 ), S( 13, 11 ), S( 14, 13 ), S( 8, 10 ), S( 15, 15 ), S( 11, 8 ),
    S( 17, 17 ), S( 16, 18 ), S( 22, 22 ), S( 23, 23 ), S( 11, 10 ), S( 18, 18 ), S( 11, 16 ), S( 18, 16 ),
    S( 5, 6 ), S( 10, 9 ), S( 6, 5 ), S( 5, 7 ), S( 1, 3 ), S( 7, 1 ), S( 10, 4 ), S( 0, -1 ),
    S( 3, 0 ), S( -3, -3 ), S( 4, 4 ), S( 0, 3 ), S( 1, -3 ), S( 15, 18 ), S( -3, 0 ), S( -1, -2 ),
    S( -11, -8 ), S( -7, 0 ), S( -2, 5 ), S( -9, -4 ), S( -7, -6 ), S( -8, -5 ), S( -4, -1 ), S( -17, -16 ),
    S( -18, -5 ), S( -12, 11 ), S( -16, -5 ), S( -19, -18 ), S( -14, -6 ), S( -11, -1 ), S( -11, -1 ), S( -29, -19 ),
    S( -18, -12 ), S( -8, -5 ), S( -12, -7 ), S( -8, -3 ), S( -12, -7 ), S( -3, -3 ), S( -12, -11 ), S( -39, 0 ),
    S( -5, -6 ), S( 1, 0 ), S( 3, 2 ), S( 6, 4 ), S( 5, 2 ), S( 14, 13 ), S( -26, -10 ), S( -22, -9 ),
};

static constexpr int queen_psqt[]
{ 
    S( -1, 3 ), S( 37, 38 ), S( 48, 49 ), S( 41, 42 ), S( 66, 66 ), S( 49, 48 ), S( 43, 41 ), S( 58, 57 ),
    S( -15, -15 ), S( -16, -9 ), S( 28, 20 ), S( 46, 43 ), S( 49, 47 ), S( 70, 70 ), S( 57, 57 ), S( 66, 58 ),
    S( -7, 3 ), S( 6, 22 ), S( 16, 18 ), S( 38, 34 ), S( 59, 57 ), S( 73, 73 ), S( 72, 72 ), S( 72, 70 ),
    S( 1, 6 ), S( -3, 11 ), S( 6, 11 ), S( 7, 22 ), S( 25, 25 ), S( 37, 38 ), S( 50, 50 ), S( 26, 27 ),
    S( 0, 13 ), S( 1, 13 ), S( 13, 17 ), S( 5, 14 ), S( 16, 23 ), S( 25, 25 ), S( 35, 32 ), S( 27, 26 ),
    S( -2, 2 ), S( 12, 13 ), S( 12, 21 ), S( 15, 20 ), S( 12, 18 ), S( 15, 21 ), S( 28, 29 ), S( 21, 23 ),
    S( -9, -8 ), S( -1, 1 ), S( 24, 21 ), S( 18, 21 ), S( 27, 27 ), S( 14, 13 ), S( -1, 3 ), S( 5, 5 ),
    S( 12, 16 ), S( 3, 5 ), S( 16, 20 ), S( 30, 24 ), S( 13, 15 ), S( -9, -9 ), S( -17, -10 ), S( -35, -33 ),
};

static constexpr int mobility_scores[4][28] 
{
  {   S(  -125, -115 ), S(  -26, -77 ), S(  -13, -29 ),
      S(  4, 9 ),       S(  21, 36 ),   S(  29, 55 ), 
      S(  36, 59 ),     S(  44, 48 ),   S(  49, 13 ) },
  
  { S( -80, -70 ),S(  -76, -67 ), S(  -18, -14 ), S(  -11, -11 ), S(  3, 4 ),   S(  12, 15 ), 
    S(  23, 21 ), S(  28, 29 )  , S(  30, 31 ),   S(  41, 35 ),   S(  39, 40 ), S(  36, 30 ), 
    S(  40, 34 ), S(  40, 39 )  , },
 
  { S( -57, -69 ), S( -28, -58 ), S( 0, -31 ), S( -13, -18 ), S( 5, 18 ), S( 5, 48 ), S( 15, 73 ),
    S( 30, 85 ),   S( 30, 82 ),   S( 32, 81 ), S( 36, 81 ),   S( 37, 82 ),S( 40, 84 ),S( 48, 66 ),
    S( 43, -10 ), },
  
  { S(-62,-48), S(-70,-36), S(-66,-49), S(-45,-50), S(-27,-46), S( -9,-45), S(  2,-37), S(  9,-23),
    S( 14, -7), S( 19,  9), S( 21, 24), S( 24, 34), S( 28, 39), S( 27, 49), S( 29, 55), S( 28, 63),
    S( 26, 69), S( 26, 69), S( 24, 73), S( 29, 72), S( 34, 74), S( 51, 63), S( 60, 69), S( 79, 66),
    S(106, 85), S(112, 84), S(104,111), S(108,131) }
};


template <PieceType type, typename Callable, typename... Args>
static constexpr int mobility_score(Callable F, Args... args)
{
    return mobility_scores[type - 1][popcount64(F(args...))];
}

template<PieceType pt>
static constexpr int safe_mobility_score(Position const& position, Color us, Square sq)
{
    uint64_t enemy_pawns = position.pieces.get_piece_bb(make_piece(Pawn, !us));
    uint64_t forward = us == White ? shift<Direction::south>(enemy_pawns) : shift<Direction::north>(enemy_pawns);
    uint64_t enemy_pawn_attacks = shift<Direction::east>(forward) | shift<Direction::west>(forward);

    uint64_t occupancy   = position.total_occupancy();
    uint64_t attacks     = Attacks::generate(pt, sq, occupancy) & ~enemy_pawn_attacks;

    return mobility_scores[pt - 1][popcount64(attacks)];
}

constexpr int passed_pawn_scores[total_ranks] = {
    S(  0, 0 ),   S(  -12, -16 ), S(  -12, -10 ), S(  7, 6 ), 
    S(  41, 42 ), S(  140, 148 ), S(  226, 228 ), S(  0, 0 ), 
};


static bool material_draw(Position const &position)
{
    auto single = [](uint64_t bb) { return bb && !is_several(bb); };

    auto &pieces = position.pieces;
    auto &bitboards = position.pieces.bitboards;

    if (bitboards[Pawn] || bitboards[Queen])
        return false;

    if (!bitboards[Rook])
    {
        if (!bitboards[Bishop])
        {
            uint64_t white = pieces.get_piece_bb<Knight>(White);
            uint64_t black = pieces.get_piece_bb<Knight>(Black);

            return popcount64(black) <= 2 && popcount64(white) <= 2;
        }

        if (!bitboards[Knight])
        {
            uint64_t white = pieces.get_piece_bb<Bishop>(White);
            uint64_t black = pieces.get_piece_bb<Bishop>(Black);

            return abs(popcount64(white) - popcount64(black)) <= 2;
        }
    }
    else
    {
        // Rook vs 2 minors
        if (single(bitboards[Rook]))
        {
            Color owner = pieces.get_piece_bb<Rook>(White) ? White : Black;

            uint64_t minor = bitboards[Bishop] | bitboards[Knight];

            return !(minor & pieces.colors[owner]) && (popcount64(minor & pieces.colors[!owner]) == 1 || popcount64(minor & pieces.colors[!owner]) == 2);
        }
    }

    return false;
}

static bool pawn_passed(uint64_t enemy_pawns, Color us, Square sq)
{
    return !(enemy_pawns & BitMask::passed_pawn[us][sq]);
}

static inline Square psqt_sq(Square sq, Color color)
{
    return color == White ? flip_square(sq) : sq;
}

// Pawn evaluation has the following aspects
//  - piece square tables -> general value of how strong a piece is on a square
//  - penalty for doubled pawns
//  - penalty for isolated pawns
//  - bonus for passed pawns. Extra bonus if the passed pawn is on a rank close to promotion
static int evaluate_pawn(Position const &position, Square sq, Color us)
{
    int score = 0;
    uint64_t enemy_pawns = position.pieces.get_piece_bb<Pawn>(!us);

    score += pawn_passed(enemy_pawns, us, sq) * passed_pawn_scores[to_int(rank_of(sq, us))];
    score += pawn_psqt[psqt_sq(sq, us)];

    return score;
}

// Knight evaluation has the following aspects
//  - piece square tables -> general value of how strong a piece is on a square
//  - Penalise immobile knights and give bonus for knights with more squares to move on
static int evaluate_knight(Position const &position, Square sq, Color us)
{
    int score = 0;

    score += knight_psqt[psqt_sq(sq, us)];
    score += safe_mobility_score<Knight>(position, us, sq);

    return score;
}

static bool is_on_open_file(Position const &position, Square sq)
{
    uint64_t file = BitMask::files[sq];
    return !(file & position.pieces.bitboards[Pawn]);
}

static bool is_on_semiopen_file(Position const &position, Square sq)
{
    uint64_t file = BitMask::files[sq];
    uint64_t white = position.pieces.get_piece_bb<Pawn>(White);
    uint64_t black = position.pieces.get_piece_bb<Pawn>(Black);

    return !(file & white) || !(file & black);
}

// Rook evaluation has the following terms
// - piece square tables -> general value of how strong a piece is on a square
// - bonus for higher mobility and penalty for low
// - bonus for a rook on a semi-open file
// - bonus for a rook on an open file
static int evaluate_rook(Position const &position, Square sq, Color us)
{
    int score = 0;

    score += rook_psqt[psqt_sq(sq, us)];
    score += mobility_score<Rook>(Attacks::rook, sq, position.total_occupancy());
    score += is_on_open_file(position, sq) * open_file_scores[Rook - 3];
    score += is_on_semiopen_file(position, sq) * semiopen_file_scores[Rook - 3];

    return score;
}

// Queen evaluation has the following terms
// - piece square tables -> general value of how strong a piece is on a square
// - bonus for higher mobility and penalty for low
// - bonus for a queen on a semi-open file
// - bonus for a queen on an open file
static int evaluate_queen(Position const &position, Square sq, Color us)
{
    int score = 0;

    score += queen_psqt[psqt_sq(sq, us)];
    score += mobility_score<Queen>(Attacks::queen, sq, position.total_occupancy());
    score += is_on_open_file(position, sq) * open_file_scores[Queen - 3];
    score += is_on_semiopen_file(position, sq) * semiopen_file_scores[Queen - 3];

    return score;
}

// Bishop evaluation has the following terms
//  - piece square tables -> general value of how strong a piece is on a square
//  - bonus for higher mobility and penalty for low
static int evaluate_bishop(Position const &position, Square sq, Color us)
{
    int score = 0;

    score += bishop_psqt[psqt_sq(sq, us)];
    score += mobility_score<Bishop>(Attacks::bishop, sq, position.total_occupancy());

    return score;
}

template <typename Callable>
static int evaluate_piece(Position const &position, Callable F, uint64_t pieces, Color us)
{
    int score = 0;
    while (pieces)
    {
        Square sq = pop_lsb(pieces);
        score += F(position, sq, us);
    }
    return score;
}

template <PieceType type, typename Callable>
static int evaluate_piece(Position const &position, Callable F)
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

static int material_balance(Position const &position)
{
    auto &pieces = position.pieces;
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

static int get_phase(Position const &position)
{
    constexpr int rook_phase = 2;
    constexpr int queen_phase = 4;

    int phase = 24;

    uint64_t knights = position.pieces.bitboards[Knight];
    uint64_t bishops = position.pieces.bitboards[Bishop];
    uint64_t rooks = position.pieces.bitboards[Rook];
    uint64_t queens = position.pieces.bitboards[Queen];

    phase -= popcount64(knights);
    phase -= popcount64(bishops);
    phase -= popcount64(queens) * queen_phase;
    phase -= popcount64(rooks) * rook_phase;

    return phase;
}

static inline int scale_score(Position const &position, int score)
{
#define mg_score(s) ((int16_t)((uint16_t)((unsigned)((s)))))
#define eg_score(s) ((int16_t)((uint16_t)((unsigned)((s) + 0x8000) >> 16)))
    int phase = get_phase(position);
    return ((mg_score(score) * (256 - phase)) + (eg_score(score) * phase)) / 256;
}

int eval_position(Position const &position)
{
    int score = 0;

    if (material_draw(position))
        return 0;

    score += material_balance(position);
    score += evaluate_piece<Pawn>(position, evaluate_pawn);
    score += evaluate_piece<Knight>(position, evaluate_knight);
    score += evaluate_piece<Rook>(position, evaluate_rook);
    score += evaluate_piece<Bishop>(position, evaluate_bishop);
    score += evaluate_piece<Queen>(position, evaluate_queen);

    score = scale_score(position, score);

    return position.side == White ? score : -score;
}