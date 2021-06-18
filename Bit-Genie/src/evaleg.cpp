#include "evaleg.h"
#include "evalscores.h"
#include "position.h"
#include "board.h"
#include <math.h>

static int get_distance(int a, int b)
{
    auto rank = [](int x) { return x / 8; };
    auto file = [](int x) { return x % 8; };
    return std::max(std::abs(rank(a) - rank(b)), std::abs(file(a) - file(b)));
}

static bool is_rook_pawn(Square sq)
{
    return file_of(sq) == File::A || file_of(sq) == File::H;
}

namespace EndGameEval
{
    bool is_king_pawn_king(Position const& position)
    {
        uint64_t queens = position.pieces.bitboards[Queen];
        uint64_t rooks  = position.pieces.bitboards[Rook];
        uint64_t knights = position.pieces.bitboards[Knight];
        uint64_t bishops = position.pieces.bitboards[Bishop];
        uint64_t pawns   = position.pieces.bitboards[Pawn];

        if (queens || rooks || knights || bishops)
            return false;

        return !is_several(pawns);
    }

    int eval_king_pawn_king(Position const& position)
    {
        uint64_t pawn       = position.pieces.bitboards[Pawn];
        Square pawn_sq      = get_lsb(pawn);
        Color  pawn_side    = color_of(position.pieces.get_piece(pawn_sq));
        Color  side_to_play = position.side;
        Square op_king_sq   = get_lsb(position.pieces.get_piece_bb<King>(!pawn_side));
        Square pawn_king_sq = get_lsb(position.pieces.get_piece_bb<King>(pawn_side));
        int win_score       = side_to_play == pawn_side ? QueenEval::value : -QueenEval::value;
        Direction forward   = pawn_side == White ? Direction::north : Direction::south;

        int distance_to_enemy_king = get_distance(pawn_sq, op_king_sq);
        int distance_to_promotion  = 7 - (int)rank_of(pawn_sq);

        if (side_to_play != pawn_side)
            distance_to_enemy_king--;

        if (distance_to_enemy_king - distance_to_promotion >= 1)
            return win_score;

        if (rank_of(pawn_sq) > rank_of(op_king_sq))
        {
            if (pawn_side == side_to_play)
                return win_score;

            if ((int)rank_of(pawn_sq) - (int)rank_of(op_king_sq) >= 2)
                return win_score;   
        }

        if (is_rook_pawn(pawn_sq))
            return -1;

        uint64_t ahead_one = shift(pawn, forward);
        uint64_t ahead_two = shift(ahead_one, forward);
        uint64_t ahead_all = ahead_one | ahead_two;

        if (test_bit(op_king_sq, ahead_all))
        {
            uint64_t sides = shift<Direction::east>(pawn) | shift<Direction::west>(pawn);

            if (rank_of(pawn_sq, pawn_side) == Rank::six 
                && test_bit(op_king_sq, ahead_two) 
                && side_to_play == pawn_side 
                && test_bit(pawn_king_sq, sides))
            {
                return win_score;
            }
            else 
                return 0;
        }

        bool our_king_in_front = test_bit(pawn_king_sq, ahead_one);
        bool king_on_sixth = rank_of(pawn_king_sq) == Rank::six;

        if (our_king_in_front && king_on_sixth)
            return win_score;

        return -1;
    }
}