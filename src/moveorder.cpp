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
    uint64_t least_valuable_attacker(Position &position, uint64_t attackers, Color side, Piece &capturing)
    {
        for (int i = 0; i < 6; i++)
        {
            PieceType pt = static_cast<PieceType>(i);
            capturing = make_piece(pt, side);
            uint64_t pieces = position.get_bb(pt, side) & attackers;

            if (pieces)     
                return pieces & (~pieces + 1);
        }
        return 0;
    }

    bool lost_material(int16_t scores[16], int index)
    {
        return (-scores[index - 1] < 0 && scores[index] < 0);
    }

    int16_t see(Position &position, Move move)
    {
        static constexpr int see_piece_vals[]{
            100, 300, 325, 500, 900, 1000,
            100, 300, 325, 500, 900, 1000, 0
        };

        int16_t scores[32] = {0};
        int index = 0;

        Square from = move.from();
        Square to = move.to();

        Piece capturing = position.get_piece(from);
        Piece captured = position.get_piece(to);
        Color attacker = color_of(capturing);

        uint64_t from_set = (1ull << from);
        uint64_t occ = position.get_bb(), bishops = 0, rooks = 0;

        bishops = rooks = position.get_bb(PieceType::Queen);
        bishops |= position.get_bb(PieceType::Bishop);
        rooks   |= position.get_bb(PieceType::Rook);

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
            occ ^= from_set;

            attack_def |= occ & ((Attacks::bishop(to, occ) & bishops) | (Attacks::rook(to, occ) & rooks));
            from_set = least_valuable_attacker(position, attack_def, attacker, capturing);
        } while (from_set);

        while (--index)
        {
            scores[index - 1] = -(-scores[index - 1] > scores[index] ? -scores[index - 1] : scores[index]);
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

                if (position.previous_move() != NullMove)
                    score += get_history(search.counter_history, position, move);

                score = std::clamp(score, -32767, 32767);

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
        position.generate_noisy(movelist);
        
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
    }
    return false;
}

bool MovePicker::next(Move &move)
{
    Position& position = *search->position;

    auto can_move = [&](Move m) 
    {
        return position.is_pseudolegal(m) && position.is_legal(m);
    };

    if (stage == Stage::HashMove)
    {
        stage = Stage::GenNoisy;
        auto& entry = TT.retrieve(position);
        
        Move hmove = Move(entry.move);

        if (entry.hash == position.get_key() && can_move(hmove))
        {
            move = hmove;
            return true;
        }
    }

    if (stage == Stage::GenNoisy)
    {
        position.generate_noisy(movelist);

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

        if (can_move(killer) && !move_is_capture(position, killer))
        {
            move = killer;
            return true;
        }
    }

    if (stage == Stage::Killer2)
    {
        stage = Stage::GiveBadNoisy;
        Move killer = search->killers[search->stats.ply][1];

        if (can_move(killer) && !move_is_capture(position, killer))
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
        position.generate_quiet(movelist);
        
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
    }
    return false;
}