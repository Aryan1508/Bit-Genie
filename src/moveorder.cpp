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

    uint64_t next_attacker(Position const& position, uint64_t attackers, Color side, Piece& capturing)
    {
        for(int i = 0;i < 6;i++)
        {
            capturing = make_piece(PieceType(i), side);
            uint64_t pieces = position.pieces.bitboards[i] & position.pieces.colors[side];
            uint64_t a = pieces & attackers;
            if (a)
                return a & -a;
        }
        return 0;
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

int16_t see(Position const& position, Move move)
{
    std::array<int16_t, 32> scores;
    uint8_t index = 0;

    Square from     = move.from(), to = move.to();
    Piece capturing = position.pieces.squares[from], captured = position.pieces.squares[to];

    uint64_t occ = position.total_bb();
    uint64_t att = Attacks::attackers_to_sq(position, to);
    uint64_t from_bb = 1ull << from;

    uint64_t bishops = position.pieces.bitboards[Bishop] | position.pieces.bitboards[Queen];
    uint64_t rooks   = position.pieces.bitboards[Rook]   | position.pieces.bitboards[Queen];

    scores[index] = see_piece_vals[captured];
    do 
    {
        index++;
        scores[index] = see_piece_vals[capturing] - scores[index - 1];

        att ^= from_bb;
        occ ^= from_bb;

        att |= occ & ((Attacks::bishop(to, occ) & bishops) | (Attacks::rook(to, occ) & rooks));

        from_bb = next_attacker(position, att, static_cast<Color>(index & 1), capturing);
    } while(from_bb);

    while(--index)
        scores[index - 1] = -std::max<int16_t>(-scores[index - 1], scores[index]);

    return scores[0];
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