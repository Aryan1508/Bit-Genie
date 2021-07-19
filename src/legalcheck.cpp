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
#include "board.h"
#include "attacks.h"
#include "position.h"

namespace 
{
    bool castle_path_is_clear(Position const& position, Square rook)
    {
        return !(position.get_bb() & BitMask::castle_piece_path[rook]);
    }
}

bool Position::is_legal(Move move) const
{
    if (move.data == 0)
        return false;

    auto from = move.from();
    auto to   = move.to();
    auto flag = move.flag();

    if (flag == Move::Flag::normal || flag == Move::Flag::promotion)
    {
        if (get_piece(from) == Piece::wKing || get_piece(from) == Piece::bKing) // Normal king moves
        {
            uint64_t occupancy = get_bb() ^ (1ull << from);
            return !Attacks::square_attacked(*this, to, !side, occupancy);
        }

        else // Normal non-king moves
        {
            uint64_t occupancy = get_bb() ^ (1ull << from) ^ (1ull << to);
            uint64_t enemy     = get_bb(!get_side());

            Piece captured = get_piece(to);

            if (captured != Piece::Empty)
            {
                occupancy ^= (1ull << to);
                enemy ^= (1ull << to);
            }

            uint64_t pawns   = get_bb(PieceType::Pawn  ) & enemy;
            uint64_t knights = get_bb(PieceType::Knight) & enemy;
            uint64_t bishops = get_bb(PieceType::Bishop) & enemy;
            uint64_t rooks   = get_bb(PieceType::Rook  ) & enemy;
            uint64_t queens  = get_bb(PieceType::Queen ) & enemy;
            
            Square king = get_lsb(get_bb(PieceType::King, get_side()));


            bishops |= queens;
            rooks |= queens;

            return !((BitMask::pawn_attacks[side][king] & pawns) || (Attacks::bishop(king, occupancy) & bishops) || (Attacks::rook(king, occupancy) & rooks) || (Attacks::knight(king) & knights));
        }
    }

    else if (move.flag() == Move::Flag::castle)
    {
        return !Attacks::square_attacked(*this, to, !side);
    }

    else
    {
        Square ep = to_sq(to ^ 8);
        uint64_t occupancy = get_bb() ^ (1ull << from) ^ (1ull << to) ^ (1ull << ep);
        uint64_t enemy     = get_bb(!get_side()) ^ (1ull << ep);

        uint64_t pawns   = get_bb(PieceType::Pawn  ) & enemy;
        uint64_t knights = get_bb(PieceType::Knight) & enemy;
        uint64_t bishops = get_bb(PieceType::Bishop) & enemy;
        uint64_t rooks   = get_bb(PieceType::Rook  ) & enemy;
        uint64_t queens  = get_bb(PieceType::Queen ) & enemy;

        Square king = get_lsb(get_bb(PieceType::King, get_side()));

        bishops |= queens;
        rooks |= queens;

        return !((BitMask::pawn_attacks[side][king] & pawns) || (Attacks::bishop(king, occupancy) & bishops) || (Attacks::rook(king, occupancy) & rooks) || (Attacks::knight(king) & knights));
    }

    return true;
}

bool Position::is_pseudolegal(Move move) const
{
    if (!move.data)
        return false;

    Square from = move.from();
    Square to = move.to();
    Move::Flag flag = move.flag();
    Piece moving   = get_piece(from);
    Piece captured = get_piece(to);

    // Moving piece can't be empty
    if (moving == Empty)
        return false;

    // Color of the piece has to be the current
    // side to play
    if (color_of(moving) != side)
        return false;

    // Cannot capture our own piece
    if (captured != Empty && color_of(captured) == side)
        return false;

    if (flag == Move::Flag::castle)
    {
        if (!test_bit(castle_rooks, to))
            return false;

        if (!castle_path_is_clear(*this, to))
            return false;

        if (Attacks::square_attacked(*this, from, !get_side()))
            return false;

        uint64_t path = BitMask::castle_attack_path[to];
        while (path)
        {
            Square sq = pop_lsb(path);
            if (Attacks::square_attacked(*this, sq, !side))
                return false;
        }

        return true;
    }

    // Validating pawn moves
    if (moving == wPawn || moving == bPawn)
    {
        Rank start_rank = side == White ? Rank::two : Rank::seven;
        Direction forward = side == White ? Direction::north : Direction::south;
        Square forward_sq = from + forward;
        uint64_t prom_rank = side == White ? BitMask::rank7 : BitMask::rank2;
        uint64_t from_sq_bb = 1ull << from;

        if (flag == Move::Flag::promotion)
        {
            if ((prom_rank & from_sq_bb) == 0)
                return false;
        }

        if (from_sq_bb & prom_rank)
        {
            if (flag != Move::Flag::promotion)
                return false;
        }

        // Normal and promotions
        if (flag == Move::Flag::normal || flag == Move::Flag::promotion)
        {
            // Pushes
            if (get_piece(to) == Empty)
            {
                Direction double_push = forward + forward;
                Square double_push_sq = from + double_push;

                // Single push
                if (to == forward_sq)
                    return true;

                // Double pushes
                return (to == double_push_sq && get_piece(forward_sq) == Empty && rank_of(from) == start_rank);
            }
            // Pawn captures
            // Already confirmed that captured piece isn't ours
            else
            {
                return test_bit(BitMask::pawn_attacks[side][from], to);
            }
        }

        // Enpassant moves
        else
        {
            if (to != ep_sq)
                return false;

            if (!((BitMask::neighbor_files[ep_sq]) & (1ull << from)))
                return false;

            captured = get_piece(static_cast<Square>(to ^ 8));

            if (captured == Empty || color_of(captured) == side)
                return false;

            Square down_right = to - forward + Direction::east;
            Square down_left = to - forward + Direction::west;

            return (from == down_left || from == down_right);
        }
    }

    // Validating knights, kings and sliding pieces
    else
    {
        // Other pieces don't have special moves
        // except for king castles, but we already checked that
        if (flag != Move::Flag::normal)
            return false;

        uint64_t attacks = Attacks::generate(type_of(moving), from, get_bb());
        return test_bit(attacks, to);
    }

    return false;
}