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
#include "evaldata.h"

#include "attacks.h"
#include "position.h"
#include <cstring>

namespace Eval
{
    
    Data::Data(Position const& position)
    {
        reset();
        init(position);
    }

    void Data::reset()
    {
        std::memset(this, 0, sizeof(Data));
    }

    void Data::init(Position const &position)
    {
        king_ring[White] = Attacks::king(get_lsb(position.pieces.get_piece_bb<King>(White)));
        king_ring[Black] = Attacks::king(get_lsb(position.pieces.get_piece_bb<King>(Black)));
    }

    void Data::update_attackers_count(uint64_t attacks, Color by)
    {
        attackers_count[by] += popcount64(attacks);
    }

    void Data::update_passers(Square sq, Color side)
    {
        set_bit(sq, passers[side]);
    }
}   
