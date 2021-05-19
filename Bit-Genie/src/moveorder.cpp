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
#include "tt.h"
#include "search.h"
#include <algorithm>

static uint64_t least_valuable_attacker(Position &position, uint64_t attackers, Color side, Piece &capturing)
{
    for (int i = 0; i < total_pieces; i++)
    {
        capturing = make_piece(PieceType(i), side);
        uint64_t pieces = position.pieces.bitboards[i] & position.pieces.colors[side] & attackers;

        if (pieces)
            return pieces & (~pieces + 1);
    }
    return 0;
}

static bool lost_material(int16_t scores[16], int index)
{
    return (-scores[index - 1] < 0 && scores[index] < 0);
}

static int16_t see(Position &position, Move move)
{
    static constexpr int see_piece_vals[]{
        100, 300, 325, 500, 900, 1000,
        100, 300, 325, 500, 900, 1000, 0};

    int16_t scores[32] = {0};
    int index = 0;

    Square from = move_from(move);
    Square to = move_to(move);

    Piece capturing = position.pieces.squares[from];
    Piece captured = position.pieces.squares[to];
    Color attacker = color_of(capturing);

    uint64_t from_set = (1ull << from);
    uint64_t occ = position.total_occupancy(), bishops = 0, rooks = 0;

    bishops = rooks = position.pieces.bitboards[Queen];
    bishops |= position.pieces.bitboards[Bishop];
    rooks |= position.pieces.bitboards[Rook];

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

static Move get_hash_move(Position &position, TTable &tt)
{
    return (Move)tt.retrieve(position).move;
}

template <bool quiet = false>
static void order_normal_movelist(Position &position, Movelist &movelist, Search &search)
{
    for (auto &move : movelist)
    {
        if constexpr (!quiet)
        {
            set_move_score(move, see(position, move));
        }
        else
            set_move_score(move, search.history.get(position, move));
    }
    std::stable_sort(movelist.begin(), movelist.end(),
              [](Move rhs, Move lhs) { return move_score(rhs) > move_score(lhs); });
}

void sort_qmovelist(Movelist &movelist, Position &position, Search &search)
{
    order_normal_movelist<false>(position, movelist, search);
}

MovePicker::MovePicker(Position &p, Search &s, TTable &tt)
    : position(&p), search(&s), table(&tt)
{
    stage = Stage::HashMove;
}

bool MovePicker::next(Move &move)
{
    auto can_move = [&](Move m) {
        return position->move_is_pseudolegal(m) && position->move_is_legal(m);
    };

    if (stage == Stage::HashMove)
    {
        stage = Stage::GenNoisy;
        Move hash_move = get_hash_move(*position, *table);

        if (can_move(hash_move))
        {
            move = hash_move;
            return true;
        }
    }

    if (stage == Stage::GenNoisy)
    {
        gen.generate<MoveGenType::noisy>(*position);
        order_normal_movelist(*position, gen.movelist, *search);

        current = gen.movelist.begin();
        stage = Stage::GiveGoodNoisy;
    }

    if (stage == Stage::GiveGoodNoisy)
    {
        if (current != gen.movelist.end() && move_score(*current) > 0)
        {
            move = *current++;
            return true;
        }
        stage = Stage::Killer1;
    }

    if (stage == Stage::Killer1)
    {
        stage = Stage::Killer2;
        Move killer = search->killers.first(search->info.ply);

        if (can_move(killer))
        {
            move = killer;
            return true;
        }
    }

    if (stage == Stage::Killer2)
    {
        stage = Stage::GiveBadNoisy;
        Move killer = search->killers.second(search->info.ply);

        if (can_move(killer))
        {
            move = killer;
            return true;
        }
    }

    if (stage == Stage::GiveBadNoisy)
    {
        if (current != gen.movelist.end())
        {
            move = *current++;
            return true;
        }
        stage = Stage::GenQuiet;
    }

    if (stage == Stage::GenQuiet)
    {
        gen.movelist.clear();
        gen.generate<MoveGenType::quiet>(*position);

        order_normal_movelist<true>(*position, gen.movelist, *search);
        current = gen.movelist.begin();
        stage = Stage::GiveQuiet;
    }

    if (stage == Stage::GiveQuiet)
    {
        if (current != gen.movelist.end())
        {
            move = *current++;
            return true;
        }
        return false;
    }
    return false;
}