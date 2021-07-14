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
#include "moveorder.h"
#include "attacks.h"
#include "tt.h"
#include "search.h"
#include <algorithm>

namespace 
{
    constexpr int see_piece_vals[]
    {
        100, 300, 325, 500, 900, 1000,
        100, 300, 325, 500, 900, 1000, 0
    };

    int next_attacker(Position const& position, uint64_t& attackers, uint64_t& occ, Color side)
    {
        for(int i = 0;i < 6;i++)
        {
            uint64_t pieces = position.pieces.bitboards[i] & position.pieces.colors[side];
            uint64_t a = pieces & attackers;

            if (a)
            {
                uint64_t x = a & -a;
                attackers ^= x;
                occ ^= x;
                return see_piece_vals[i];
            }
        }
        return 0;
    }

    int16_t see(Position const& position, Move move)
    {
        Square from = move.from();
        Square to  =  move.to();

        Piece attacker = position.pieces.squares[from];
        Piece victim   = position.pieces.squares[to];
        uint64_t occ = position.total_bb() ^ (1ull << from);

        int16_t score = see_piece_vals[victim];
        uint64_t attackers = Attacks::attackers_to_sq(position, to) ^ (1ull << from);
        Color side = !position.side;

        int a_val = see_piece_vals[attacker];
        uint64_t bishops  =0, rooks = 0;
        bishops = rooks = position.pieces.bitboards[Queen];
        bishops |= position.pieces.bitboards[Bishop];
        rooks   |= position.pieces.bitboards[Rook];

        int16_t scores[16] = {0};
        int16_t index = 0;
        scores[index++] = score;

        while(true)
        {
            int next = next_attacker(position, attackers, occ, side);
            if (!next) break;
            
            score += (position.side == side ? a_val : -a_val);
            a_val = next;

            scores[index++] = score;        

            attackers |= occ & ((Attacks::bishop(to, occ) & bishops) | (Attacks::rook(to, occ) & rooks));
            side = !side;
        }

        while(--index)
        {
            side = !side;
            if (position.side == side)
                scores[index - 1] = std::max<int16_t>(scores[index - 1], scores[index]);
            else 
                scores[index - 1] = std::min<int16_t>(scores[index - 1], scores[index]);
        }

        return scores[0];
    }

    template <bool quiet = false>
    void score_movelist(Movelist &movelist, Search::Info& search)
    {
        Position& position = *search.position;
        for (auto &move : movelist)
        {
            if constexpr (!quiet)
            {
                int score = see(position, move);

                score += get_history(search.capture_history, position, move) / 128;

                if (move.flag() == Move::Flag::promotion)
                    score += move.promoted() == PieceType::Queen ? 1000 : 300;

                move.score = score;
            }
            else
            {
                int score = get_history(search.history, position, move);

                if (move.flag() == Move::Flag::promotion)
                    score += 10000;

                move.score = score;
            }
        }
    }

    void bubble_top_move(Movelist::iterator begin, Movelist::iterator end)
    {  
        auto best = std::max_element(begin, end);
        std::iter_swap(best, begin);
    }
}

MovePicker::MovePicker(Search::Info& s)
    : search(&s)
{
    stage = Stage::HashMove;
}

bool MovePicker::qnext(Move &move)
{
    Position& position = *search->position;

    if (stage == Stage::HashMove) 
    {
        position.generate_noisy_moves(movelist);
        
        score_movelist<false>(movelist, *search);
        bubble_top_move(movelist.begin(), movelist.end());
        current = movelist.begin();

        stage = Stage::GiveGoodNoisy;
    }

    if (stage == Stage::GiveGoodNoisy)
    {
        stage = Stage::GenQuiet;
        if (current != movelist.end() && current->score >= 0)
        {
            move = *current++;
            bubble_top_move(current, movelist.end());
            return true;
        }
        return false;
    }
    return false;
}

bool MovePicker::next(Move &move)
{
    Position& position = *search->position;

    auto can_move = [&](Move m) 
    {
        return position.move_is_pseudolegal(m) && position.move_is_legal(m);
    };

    if (stage == Stage::HashMove)
    {
        stage = Stage::GenNoisy;
        auto& entry = TT.retrieve(position);
        
        Move hmove = Move(entry.move);

        if (entry.hash == position.key.data() && can_move(hmove))
        {
            move = hmove;
            return true;
        }
    }

    if (stage == Stage::GenNoisy)
    {
        position.generate_noisy_moves(movelist);

        score_movelist(movelist, *search);
        bubble_top_move(movelist.begin(), movelist.end());
        current = movelist.begin();

        stage = Stage::GiveGoodNoisy;
    }

    if (stage == Stage::GiveGoodNoisy)
    {
        if (current != movelist.end() && current->score >= 0)
        {
            move = *current++;
            bubble_top_move(current, movelist.end());
            return true;
        }
        stage = Stage::Killer1;
    }

    if (stage == Stage::Killer1)
    {
        stage = Stage::Killer2;
        Move killer = search->killers[search->stats.ply][0];

        if (can_move(killer))
        {
            move = killer;
            return true;
        }
    }

    if (stage == Stage::Killer2)
    {
        stage = Stage::GiveBadNoisy;
        Move killer = search->killers[search->stats.ply][1];

        if (can_move(killer))
        {
            move = killer;
            return true;
        }
    }

    if (stage == Stage::GiveBadNoisy)
    {
        if (current != movelist.end())
        {
            move = *current++;
            bubble_top_move(current, movelist.end());
            return true;
        }
        stage = Stage::GenQuiet;
    }

    if (stage == Stage::GenQuiet && !skip_quiets)
    {
        movelist.clear();
        position.generate_quiet_moves(movelist);
        
        score_movelist<true>(movelist, *search);
        bubble_top_move(movelist.begin(), movelist.end());
        current = movelist.begin();
        stage = Stage::GiveQuiet;
    }

    if (stage == Stage::GiveQuiet && !skip_quiets)
    {
        if (current != movelist.end())
        {
            move = *current++;
            bubble_top_move(current, movelist.end());
            return true;
        }
        return false;
    }
    return false;
}