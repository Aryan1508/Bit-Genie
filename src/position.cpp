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
#include "position.h"
#include "attacks.h"
#include "stringparse.h"

Position::Position()
{
    set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

std::ostream& operator<<(std::ostream& o, Position const& position)
{
    return o << position.get_fen();
}

bool Position::king_in_check() const 
{
    uint64_t king = get_bb(PieceType::King, side);
    return Attacks::square_attacked(*this, get_lsb(king), !side);
}

bool Position::drawn() const 
{
    for(int i = history_ply - 2;i >= 0 && i >= history_ply - halfmoves;i -= 2)
    {
        if (history[i].key == key)
            return true;
    }

    if (halfmoves >= 100) return true;

    auto const& bbs = bitboards;

    if (bbs[Pawn] || bbs[Rook] || bbs[Queen])
        return false;

    if (bbs[Knight])
    {
        if (bbs[Bishop])
            return false;

        return popcount64(bbs[Knight]) <= 2;
    }

    return   !is_several(bbs[Bishop] & get_bb(White))
          && !is_several(bbs[Bishop] & get_bb(Black));
}