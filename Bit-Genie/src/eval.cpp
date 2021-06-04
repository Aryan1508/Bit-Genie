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
#include "evalscores.h"
#include <cstring>
struct EvalData
{
    int king_attackers_count[2] = {0};
    int king_attackers_weight[2] = {0};
    uint64_t king_ring[2] = {0};

    void init(Position const &position)
    {
        std::memset(this, 0, sizeof(EvalData));
        king_ring[White] = Attacks::king(get_lsb(position.pieces.get_piece_bb<King>(White)));
        king_ring[Black] = Attacks::king(get_lsb(position.pieces.get_piece_bb<King>(Black)));
    }
};

template <PieceType pt, bool safe = false>
static constexpr int calculate_moblity(Position const &position, EvalData &data, Square sq, Color us, const int *mobility_scores)
{
    uint64_t occupancy = position.total_occupancy();
    uint64_t attacks = Attacks::generate(pt, sq, occupancy);

    if constexpr (!safe)
    {
        if (attacks & data.king_ring[!us])
        {
            data.king_attackers_weight[us] += KingEval::attack_weight[pt] * popcount64(attacks & data.king_ring[!us]);
            data.king_attackers_count[us]++;
        }
        return mobility_scores[popcount64(attacks)];
    }
    else
    {
        uint64_t enemy_pawns = position.pieces.get_piece_bb<Pawn>(!us);
        uint64_t forward = us == White ? shift<Direction::south>(enemy_pawns) : shift<Direction::north>(enemy_pawns);
        uint64_t enemy_pawn_attacks = shift<Direction::east>(forward) | shift<Direction::west>(forward);

        if (attacks & data.king_ring[!us])
        {
            data.king_attackers_weight[us] += KingEval::attack_weight[pt] * popcount64(attacks & data.king_ring[!us]);
            data.king_attackers_count[us]++;
        }

        return mobility_scores[popcount64(attacks & ~enemy_pawn_attacks)];
    }
}

static bool material_draw(Position const &position)
{
    auto single = [](uint64_t bb)
    { return bb && !is_several(bb); };

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

static bool pawn_is_isolated(uint64_t friend_pawns, Square sq)
{
    return !(friend_pawns & BitMask::neighbor_files[sq]);
}

static bool pawn_is_stacked(uint64_t friend_pawns, Square sq)
{
    uint64_t sq_bb = 1ull << sq;
    return (shift<Direction::north>(sq_bb) & friend_pawns) | (shift<Direction::south>(sq_bb) & friend_pawns);
}

static Square psqt_sq(Square sq, Color color)
{
    return color == White ? flip_square(sq) : sq;
}

static int evaluate_pawn(Position const &position, EvalData, Square sq, Color us)
{
    int score = 0;
    uint64_t enemy_pawns = position.pieces.get_piece_bb<Pawn>(!us);
    uint64_t friend_pawns = position.pieces.get_piece_bb<Pawn>(us);

    score += pawn_passed(enemy_pawns, us, sq) * PawnEval::passed[psqt_sq(sq, us)];
    score += PawnEval::psqt[psqt_sq(sq, us)];
    score += pawn_is_isolated(friend_pawns, sq) * PawnEval::isolated;
    score += pawn_is_stacked(friend_pawns, sq) * PawnEval::stacked;

    return score;
}

static int evaluate_knight(Position const &position, EvalData &data, Square sq, Color us)
{
    int score = 0;

    score += KnightEval::psqt[psqt_sq(sq, us)];
    score += calculate_moblity<Knight, true>(position, data, sq, us, KnightEval::mobility);

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

static int evaluate_rook(Position const &position, EvalData &data, Square sq, Color us)
{
    int score = 0;

    score += RookEval::psqt[psqt_sq(sq, us)];
    score += calculate_moblity<Rook>(position, data, sq, us, RookEval::mobility);
    score += is_on_open_file(position, sq) * RookEval::open_file;
    score += is_on_semiopen_file(position, sq) * RookEval::semi_open_file;

    return score;
}

static int evaluate_queen(Position const &position, EvalData &data, Square sq, Color us)
{
    int score = 0;

    score += QueenEval::psqt[psqt_sq(sq, us)];
    score += calculate_moblity<Queen>(position, data, sq, us, QueenEval::mobility);
    score += is_on_open_file(position, sq) * QueenEval::open_file;
    score += is_on_semiopen_file(position, sq) * QueenEval::semi_open_file;

    return score;
}

static int evaluate_bishop(Position const &position, EvalData &data, Square sq, Color us)
{
    int score = 0;

    score += BishopEval::psqt[psqt_sq(sq, us)];
    score += calculate_moblity<Bishop>(position, data, sq, us, BishopEval::mobility);

    return score;
}

template <typename Callable>
static int evaluate_piece(Position const &position, EvalData &data, Callable F, uint64_t pieces, Color us)
{
    int score = 0;
    while (pieces)
    {
        Square sq = pop_lsb(pieces);
        score += F(position, data, sq, us);
    }
    return score;
}

template <PieceType type, typename Callable>
static int evaluate_piece(Position const &position, EvalData &data, Callable F)
{
    int score = 0;

    uint64_t white = position.pieces.bitboards[type] & position.pieces.colors[White];
    uint64_t black = position.pieces.bitboards[type] & position.pieces.colors[Black];

    score += evaluate_piece(position, data, F, white, Color::White);
    score -= evaluate_piece(position, data, F, black, Color::Black);
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

static int eval_king(Position const &position, EvalData &data, Color us)
{
    Square sq = get_lsb(position.pieces.get_piece_bb<King>(us));

    int score = KingEval::psqt[psqt_sq(sq, us)];
    Color enemy = !us;

    if (data.king_attackers_count[enemy] >= 2)
    {
        int weight = data.king_attackers_weight[enemy];

        if (!position.pieces.get_piece_bb<Queen>(enemy))
            weight /= 2;

        score += KingEval::safety_table[weight];
    }

    return score;
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

    EvalData data;
    data.init(position);

    score += material_balance(position);
    score += evaluate_piece<Pawn>(position, data, evaluate_pawn);
    score += evaluate_piece<Knight>(position, data, evaluate_knight);
    score += evaluate_piece<Rook>(position, data, evaluate_rook);
    score += evaluate_piece<Bishop>(position, data, evaluate_bishop);
    score += evaluate_piece<Queen>(position, data, evaluate_queen);

    score += eval_king(position, data, White);
    score -= eval_king(position, data, Black);

    score = scale_score(position, score);

    return position.side == White ? score : -score;
}
