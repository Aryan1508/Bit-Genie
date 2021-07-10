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
#include "misc.h"

namespace Eval 
{
    struct Data
    {
        int king_attackers_count[2] = {0};
        int king_attackers_weight[2] = {0};
        uint64_t king_ring[2] = {0};
        int attackers_count[2];

        Data(Position const&);

        void reset();
        void init(Position const &position);
        void update_attackers_count(uint64_t attacks, Color by);
    };
}