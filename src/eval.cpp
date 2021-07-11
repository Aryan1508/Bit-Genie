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

#include "board.h"
#include "piece.h"
#include "attacks.h"
#include "evaldata.h"
#include "position.h"
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

namespace 
{
    template <PieceType pt, bool safe = false>
    int calculate_moblity(Position const &position, Eval::Data &data, Square sq, Color us, const int *mobility_scores)
    {
        uint64_t occupancy = position.total_bb();
        
        if (pt == Rook)
            occupancy ^= position.pieces.get_piece_bb<Rook>(us);

        uint64_t attacks = Attacks::generate(pt, sq, occupancy);
        data.update_attackers_count(attacks, us);

        if (attacks & data.king_ring[!us])
        {
            data.king_attackers_weight[us] += KING_ATTACK_WEIGHT[pt] * popcount64(attacks & data.king_ring[!us]);
            data.king_attackers_count[us]++;
        }

        if constexpr (safe)
        {
            uint64_t enemy_pawns = position.pieces.get_piece_bb<Pawn>(!us);
            uint64_t attacked    = Attacks::pawn(enemy_pawns, !us); 

            int count = popcount64(attacks & ~attacked);
            TRACE_3(mobility, pt, count);
            return mobility_scores[count];
        }
        else  
        {
            int count = popcount64(attacks);
            TRACE_3(mobility, pt, count);
            return mobility_scores[count];
        }
    }

    int evaluate_pawn(Position const &position, Eval::Data& data, Square sq, Color us)
    {   
        auto pawn_is_passed = 
        [](uint64_t enemy_pawns, Color us, Square sq)
        {
            return !(enemy_pawns & BitMask::passed_pawn[us][sq]);
        };

        auto pawn_is_stacked = 
        [](uint64_t friend_pawns, Square sq)
        {
            uint64_t sq_bb = 1ull << sq;
            return (shift<Direction::north>(sq_bb) & friend_pawns) | (shift<Direction::south>(sq_bb) & friend_pawns);
        };

        auto pawn_is_isolated = 
        [](uint64_t friend_pawns, Square sq)
        {
            return !(friend_pawns & BitMask::neighbor_files[sq]);
        };

        int score = 0;
        uint64_t enemy_pawns = position.pieces.get_piece_bb<Pawn>(!us);
        uint64_t friend_pawns = position.pieces.get_piece_bb<Pawn>(us);
        uint64_t enemy = position.pieces.get_occupancy(!us);
        uint64_t ahead_squares = BitMask::passed_pawn[us][sq] & BitMask::files[sq];
        Square relative_sq = psqt_sq(sq, us);

        data.update_attackers_count(BitMask::pawn_attacks[us][sq], us);

        score += PAWN_PSQT[relative_sq];
        TRACE_3(psqt, Pawn, relative_sq);

        if (pawn_is_isolated(friend_pawns, sq))
        {
            TRACE_1(isolated);
            score += PAWN_ISOLATED;
        }

        if (pawn_is_stacked(friend_pawns, sq))
        {
            TRACE_1(stacked);
            score += PAWN_STACKED;
        }

        if (pawn_is_passed(enemy_pawns, us, sq))
        {
            data.update_passers(sq, us);
            if (ahead_squares & enemy)
            {
                score += BLOCKED_PASSER[relative_sq];
                TRACE_2(blocked_passer, relative_sq);
            }
            else 
            {
                score += PASSER[relative_sq];
                TRACE_2(passer, relative_sq);

                if (position.side == us)
                {
                    TRACE_1(passer_tempo);
                    score += PASSER_TEMPO;
                }
            }

            if (BitMask::pawn_attacks[!us][sq] & friend_pawns)
            {
                score += SUPPORTED_PASSER;
                TRACE_1(supported_passer);
            }
        }

        score += PAWN_VALUE;
        TRACE_2(material, Pawn);

        return score;
    }

    int evaluate_knight(Position const &position, Eval::Data &data, Square sq, Color us)
    {
        int score = 0;
        Square relative_sq = psqt_sq(sq, us);

        score += KNIGHT_PSQT[relative_sq];
        TRACE_3(psqt, Knight, relative_sq);
        
        score += calculate_moblity<Knight, true>(position, data, sq, us, KNIGHT_MOBILITY);

        score += KNIGHT_VALUE;
        TRACE_2(material, Knight);

        return score;
    }

    int evaluate_bishop(Position const &position, Eval::Data &data, Square sq, Color us)
    {
        int score = 0;
        Square relative_sq = psqt_sq(sq, us);

        score += BISHOP_PSQT[relative_sq];
        TRACE_3(psqt, Bishop, relative_sq);

        score += calculate_moblity<Bishop>(position, data, sq, us, BISHOP_MOBILITY);

        score += BISHOP_VALUE;
        TRACE_2(material, Bishop);

        return score;
    }

    int evaluate_rook(Position const &position, Eval::Data &data, Square sq, Color us)
    {
        auto is_on_semi_open_file = 
        [](Position const &position, Square sq)
        {
            uint64_t file  = BitMask::files[sq];
            uint64_t white = position.pieces.get_piece_bb<Pawn>(White);
            uint64_t black = position.pieces.get_piece_bb<Pawn>(Black);

            return !(file & white) || !(file & black);
        };

        auto is_on_open_file = 
        [](Position const &position, Square sq)
        {
            uint64_t file = BitMask::files[sq];
            return !(file & position.pieces.bitboards[Pawn]);
        };

        int score = 0;
        Square relative_sq = psqt_sq(sq, us);
        uint64_t friend_rooks = position.pieces.get_piece_bb<Rook>(us) ^ (1ull << sq);

        score += ROOK_PSQT[relative_sq];
        TRACE_3(psqt, Rook, relative_sq);

        score += calculate_moblity<Rook>(position, data, sq, us, ROOK_MOBILITY);

        if (is_on_open_file(position, sq))
        {
            TRACE_1(open_file);
            score += OPEN_FILE;
        }

        if (is_on_semi_open_file(position, sq))
        {
            TRACE_1(semi_open_file);
            score += SEMI_OPEN_FILE;
        }

        if (BitMask::files[sq] & friend_rooks)
        {
            TRACE_1(friendly_file);
            score += FRIENDLY_FILE;
        }

        if (BitMask::ranks[sq] & friend_rooks)
        {
            TRACE_1(friendly_rank);
            score += FRIENDLY_RANK;
        }

        score += ROOK_VALUE;
        TRACE_2(material, Rook);

        return score;
    }

    int evaluate_queen(Position const &position, Eval::Data &data, Square sq, Color us)
    {
        int score = 0;
        Square relative_sq = psqt_sq(sq, us);

        score += QUEEN_PSQT[relative_sq];
        TRACE_3(psqt, Queen, relative_sq);

        score += calculate_moblity<Queen>(position, data, sq, us, QUEEN_MOBILITY);

        score += QUEEN_VALUE;
        TRACE_2(material, Queen);

        return score;
    }

    int evaluate_king(Position const &position, Eval::Data &data, Square sq, Color us)
    {
        Square relative_sq = psqt_sq(sq, us);
        uint64_t friend_pawns = position.pieces.get_piece_bb<Pawn>(us);

        int score = KING_PSQT[relative_sq];    
        TRACE_3(psqt, King, relative_sq);

        Color enemy = !us;

        data.update_attackers_count(BitMask::king_attacks[sq], us);

        if (data.king_attackers_count[enemy] >= 2)
        {
            int weight = data.king_attackers_weight[enemy];

            if (!position.pieces.get_piece_bb<Queen>(enemy))
                weight /= 2;

            score += KING_SAFETY_TABLE[weight];
            TRACE_2(safety_table, weight);
        }

        int shield_pawn_count = popcount64(friend_pawns & BitMask::pawn_shield[us][sq]);
        score += PAWN_SHIELD[shield_pawn_count];
        TRACE_2(pawn_shield, shield_pawn_count);

        return score;
    }

    template <typename Callable>
    int evaluate_piece(Position const &position, Eval::Data &data, Callable F, uint64_t pieces, Color us)
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
    int evaluate_piece(Position const &position, Eval::Data &data, Callable F)
    {
        int score = 0;

        uint64_t white = position.pieces.get_piece_bb<type>(White);
        uint64_t black = position.pieces.get_piece_bb<type>(Black);

        score += evaluate_piece(position, data, F, white, Color::White);
        score -= evaluate_piece(position, data, F, black, Color::Black);
        return score;
    }

    int scale_score(Position const &position, int score)
    {
        int phase = Eval::get_phase(position);
        int scale = Eval::get_scale_factor(position, score);
        return ((mg_score(score) * (256 - phase)) + (eg_score(score) * phase * (scale / 128.0f))) / 256;
    }

    int is_ocb(Position const& position)
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
    int evaluate_control(Eval::Data& data)
    {
        int count = data.attackers_count[us] - data.attackers_count[!us];
        TRACE_COUNT(control, count);
        return CONTROL * count;    
    }

    template<Color us>
    int evaluate_pawn_structure(Position const& position, Eval::Data& data)
    {
        int score = 0;
        uint64_t pawns = position.pieces.get_piece_bb<Pawn>(us);

        int supported = popcount64(pawns & Attacks::pawn(pawns, !us));
        score += supported * PAWN_SUPPORT;
        TRACE_COUNT(support, supported);

        uint64_t passers = data.passers[us];
        uint64_t connected = shift<Direction::east>(passers) & passers;
        int connected_count = popcount64(connected);

        score += connected_count * CONNECTED_PASSER;
        TRACE_COUNT(connected_passer, connected_count);
        
        return score;
    }
}

namespace Eval
{
    int get_phase(Position const &position)
    {
        uint64_t knights = position.pieces.bitboards[Knight];
        uint64_t bishops = position.pieces.bitboards[Bishop];
        uint64_t rooks   = position.pieces.bitboards[Rook];
        uint64_t queens  = position.pieces.bitboards[Queen];

        int phase = 24 - 1 * popcount64(knights | bishops)
                       - 2 * popcount64(rooks)
                       - 4 * popcount64(queens);
        phase = (phase * 256 + 12) / 24;

        return phase;
    }

    int get_scale_factor(Position const& position, int eval)
    {
        if (is_ocb(position))
            return 64;

        Color stronger = eval > 0 ? White : Black;
        uint64_t pawns = position.pieces.get_piece_bb<Pawn>(stronger);
        int count =  8 - popcount64(pawns);

        return 128 - count * count;
    }

    int evaluate(Position const &position)
    {
        int score = 0;

        Data data(position);

        score += evaluate_piece<Pawn  >(position, data, evaluate_pawn  );
        score += evaluate_piece<Knight>(position, data, evaluate_knight);
        score += evaluate_piece<Rook  >(position, data, evaluate_rook  );
        score += evaluate_piece<Bishop>(position, data, evaluate_bishop);
        score += evaluate_piece<Queen >(position, data, evaluate_queen );
        score += evaluate_piece<King  >(position, data, evaluate_king  );

        score += evaluate_control<White>(data);
        score -= evaluate_control<Black>(data); 

        score += evaluate_pawn_structure<White>(position, data);
        score -= evaluate_pawn_structure<Black>(position, data);

        TRACE_VAL(eval, score);
        score = scale_score(position, score);

        return position.side == White ? score : -score;
    }
}