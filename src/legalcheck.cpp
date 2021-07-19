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
#include "attacks.h"
#include "position.h"

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

