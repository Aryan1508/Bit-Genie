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
#include <math.h>
#include <cstring>

#ifdef TUNE 
constexpr int SIDE_SCALAR[] = {1, -1};
#define TRACE_1(term) (ET.term += SIDE_SCALAR[us])
#define TRACE_2(term, i) (ET.term[(i)] += SIDE_SCALAR[us])
#define TRACE_3(term, i, j) (ET.term[(i)][(j)] += SIDE_SCALAR[us])
#define TRACE_COUNT(term, count) (ET.term += SIDE_SCALAR[us] * (count))
#define TRACE_VAL(term, value) (ET.term = (value))
#else 
#define TRACE_1(term) 
#define TRACE_2(term, i) 
#define TRACE_3(term, i, j) 
#define TRACE_VAL(term, value)
#define TRACE_COUNT(term, count) 
#endif 

struct EvalData
{
    int king_attackers_count[2] = {0};
    int king_attackers_weight[2] = {0};
    uint64_t king_ring[2] = {0};
    uint64_t inner_king_ring[2] = {0};
    uint64_t outer_king_ring[2] = {0};
    int attackers_count[2];

    void init(Position const &position)
    {
        std::memset(this, 0, sizeof(EvalData));

        uint64_t wring = Attacks::king(get_lsb(position.pieces.get_piece_bb<King>(White)));
        uint64_t bring = Attacks::king(get_lsb(position.pieces.get_piece_bb<King>(Black)));

        inner_king_ring[White] = wring;
        inner_king_ring[Black] = bring;

        wring |= shift<Direction::north>(wring);
        bring |= shift<Direction::south>(bring);

        outer_king_ring[White] = wring & ~inner_king_ring[White];
        outer_king_ring[Black] = bring & ~inner_king_ring[Black];

        king_ring[White] = wring;
        king_ring[Black] = bring;
    }

    void update_attackers_count(uint64_t attacks, Color by)
    {
        attackers_count[by] += popcount64(attacks);
    }
};

template <PieceType pt, bool safe = false>
static constexpr int calculate_moblity(Position const &position, EvalData &data, Square sq, Color us, const int *mobility_scores)
{
    uint64_t occupancy = position.total_bb();
    uint64_t attacks = Attacks::generate(pt, sq, occupancy);

    data.update_attackers_count(attacks, us);

    if constexpr (!safe)
    {
        if (attacks & data.king_ring[!us])
        {
            data.king_attackers_weight[us] += KingEval::attack_weight[pt] * popcount64(attacks & data.inner_king_ring[!us]);
            data.king_attackers_weight[us] += (KingEval::attack_weight[pt] * popcount64(attacks & data.outer_king_ring[!us])) / 2;
            data.king_attackers_count[us]++;
        }

        int count = popcount64(attacks);
        TRACE_3(mobility, pt, count);

        return mobility_scores[count];
    }
    else
    {
        uint64_t enemy_pawns = position.pieces.get_piece_bb<Pawn>(!us);
        uint64_t forward = us == White ? shift<Direction::south>(enemy_pawns) : shift<Direction::north>(enemy_pawns);
        uint64_t enemy_pawn_attacks = shift<Direction::east>(forward) | shift<Direction::west>(forward);

        if (attacks & data.king_ring[!us])
        {
            data.king_attackers_weight[us] += KingEval::attack_weight[pt] * popcount64(attacks & data.inner_king_ring[!us]);
            data.king_attackers_weight[us] += (KingEval::attack_weight[pt] * popcount64(attacks & data.outer_king_ring[!us])) / 2;
            data.king_attackers_count[us]++;
        }

        int count = popcount64(attacks & ~enemy_pawn_attacks);
        TRACE_3(mobility, pt, count);

        return mobility_scores[count];
    }
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

static int evaluate_pawn(Position const &position, EvalData& data, Square sq, Color us)
{
    int score = 0;
    uint64_t enemy_pawns = position.pieces.get_piece_bb<Pawn>(!us);
    uint64_t friend_pawns = position.pieces.get_piece_bb<Pawn>(us);
    uint64_t enemy = position.pieces.get_occupancy(!us);
    uint64_t ahead_squares = BitMask::passed_pawn[us][sq] & BitMask::files[sq];
    Square relative_sq = psqt_sq(sq, us);

    data.update_attackers_count(BitMask::pawn_attacks[us][sq], us);

    score += PawnEval::psqt[relative_sq];
    TRACE_3(psqt, Pawn, relative_sq);

    if (pawn_is_isolated(friend_pawns, sq))
    {
        TRACE_1(isolated);
        score += PawnEval::isolated;
    }

    if (pawn_is_stacked(friend_pawns, sq))
    {
        TRACE_1(stacked);
        score += PawnEval::stacked;
    }

    if (pawn_passed(enemy_pawns, us, sq))
    {
        if (ahead_squares & enemy)
        {
            score += PawnEval::passer_blocked[relative_sq];
            TRACE_2(passer_blocked, relative_sq);
        }
        else 
        {
            score += PawnEval::passed[relative_sq];
            TRACE_2(passed, relative_sq);

            if (position.side == us)
            {
                TRACE_1(passed_tempo);
                score += PawnEval::passed_tempo;
            }
        }

        if (BitMask::pawn_attacks[!us][sq] & friend_pawns)
        {
            score += PawnEval::passed_connected;
            TRACE_1(passed_connected);
        }
    }

    score += PawnEval::value;
    TRACE_2(material, Pawn);

    return score;
}

static int evaluate_knight(Position const &position, EvalData &data, Square sq, Color us)
{
    int score = 0;
    Square relative_sq = psqt_sq(sq, us);

    score += KnightEval::psqt[relative_sq];
    TRACE_3(psqt, Knight, relative_sq);
    

    score += calculate_moblity<Knight, true>(position, data, sq, us, KnightEval::mobility);

    score += KnightEval::value;
    TRACE_2(material, Knight);

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
    Square relative_sq = psqt_sq(sq, us);

    score += RookEval::psqt[relative_sq];
    TRACE_3(psqt, Rook, relative_sq);

    score += calculate_moblity<Rook>(position, data, sq, us, RookEval::mobility);

    if (is_on_open_file(position, sq))
    {
        TRACE_1(open_file);
        score += RookEval::open_file;
    }

    if (is_on_semiopen_file(position, sq))
    {
        TRACE_1(semi_open_file);
        score += RookEval::semi_open_file;
    }

    score += RookEval::value;
    TRACE_2(material, Rook);

    return score;
}

static int evaluate_queen(Position const &position, EvalData &data, Square sq, Color us)
{
    int score = 0;
    Square relative_sq = psqt_sq(sq, us);

    score += QueenEval::psqt[relative_sq];
    TRACE_3(psqt, Queen, relative_sq);

    score += calculate_moblity<Queen>(position, data, sq, us, QueenEval::mobility);

    score += QueenEval::value;
    TRACE_2(material, Queen);

    return score;
}

static int evaluate_bishop(Position const &position, EvalData &data, Square sq, Color us)
{
    int score = 0;
    Square relative_sq = psqt_sq(sq, us);

    score += BishopEval::psqt[relative_sq];
    TRACE_3(psqt, Bishop, relative_sq);

    score += calculate_moblity<Bishop>(position, data, sq, us, BishopEval::mobility);

    score += BishopEval::value;
    TRACE_2(material, Bishop);

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

int get_phase(Position const &position)
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
    phase = (phase * 256 + 12) / 24;

    return phase;
}

static int evaluate_king(Position const &position, EvalData &data, Square sq, Color us)
{
    Square relative_sq = psqt_sq(sq, us);
    uint64_t friend_pawns = position.pieces.get_piece_bb<Pawn>(us);

    int score = KingEval::psqt[relative_sq];    
    TRACE_3(psqt, King, relative_sq);

    Color enemy = !us;

    data.update_attackers_count(BitMask::king_attacks[sq], us);

    if (data.king_attackers_count[enemy] >= 2)
    {
        int weight = data.king_attackers_weight[enemy];

        if (!position.pieces.get_piece_bb<Queen>(enemy))
            weight /= 2;

        weight = std::min(99, weight);

        score += KingEval::safety_table[weight];
        TRACE_2(safety_table, weight);
    }

    int shield_pawn_count = popcount64(friend_pawns & BitMask::pawn_shield[us][sq]);
    score += KingEval::pawn_shield[shield_pawn_count];
    TRACE_2(pawn_shield, shield_pawn_count);

    return score;
}

template<Color us>
static inline int evaluate_control(EvalData& data)
{
    int count = data.attackers_count[us] - data.attackers_count[!us];
    TRACE_COUNT(control, count);
    return MiscEval::control * count;    
}

static inline int scale_score(Position const &position, int score)
{
    int phase = get_phase(position);
    int scale = scale_factor(position, score);

    return ((mg_score(score) * (256 - phase)) + (eg_score(score) * phase * (scale / 128.0f))) / 256;
}

static inline int is_ocb(Position const& position)
{
    constexpr uint64_t dark_squares = 0xAA55AA55AA55AA55;

    uint64_t rooks   = position.pieces.bitboards[Rook];
    uint64_t knights = position.pieces.bitboards[Knight];
    uint64_t queens  = position.pieces.bitboards[Queen];
    uint64_t bishopw = position.pieces.get_piece_bb<Bishop>(White);
    uint64_t bishopb = position.pieces.get_piece_bb<Bishop>(Black);
    uint64_t bishops = bishopw | bishopb;

    if (rooks || knights || queens) return false;

    return popcount64(bishopw) == 1 
        && popcount64(bishopb) == 1 
        && popcount64(bishops & dark_squares) == 1;
}

template<Color us>
static int evaluate_pawn_structure(Position const& position)
{
    int score = 0;

    uint64_t pawns = position.pieces.get_piece_bb<Pawn>(us);

    int supported = popcount64(pawns & Attacks::pawn(pawns, !us));
    score += supported * PawnEval::support;
    TRACE_COUNT(support, supported);
   
    return score;
}

int scale_factor(Position const& position, int eval)
{
    if (is_ocb(position))
        return 64;

    Color stronger = eval > 0 ? White : Black;

    uint64_t pawns = position.pieces.get_piece_bb<Pawn>(stronger);
    int count =  8 - popcount64(pawns);

    return 128 - count * count;
}

int eval_position(Position const &position)
{
    int score = 0;

    EvalData data;
    data.init(position);

    score += evaluate_piece<Pawn  >(position, data, evaluate_pawn);
    score += evaluate_piece<Knight>(position, data, evaluate_knight);
    score += evaluate_piece<Rook  >(position, data, evaluate_rook);
    score += evaluate_piece<Bishop>(position, data, evaluate_bishop);
    score += evaluate_piece<Queen >(position, data, evaluate_queen);
    score += evaluate_piece<King  >(position, data, evaluate_king);

    score += evaluate_control<White>(data);
    score -= evaluate_control<Black>(data); 

    score += evaluate_pawn_structure<White>(position);
    score -= evaluate_pawn_structure<Black>(position);

    TRACE_VAL(eval, score);

    score = scale_score(position, score);

    return position.side == White ? score : -score;
}
