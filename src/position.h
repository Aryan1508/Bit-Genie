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
#include "fixed_list.h"
#include "position_undo.h"
#include <string_view>

class Position
{
public:
    // Set the default fen 
    Position();

    // Set a fen, does not do any validation (might crash on bad fens)
    void set_fen(std::string_view);

    // convert position to fen
    std::string get_fen();
private:
    std::array<uint64_t,  6> bitboards;
    std::array<uint64_t,  2> colors;
    std::array<Square  , 64> pieces; 

    ZobristKey key;
    Square     ep_sq;
    Color      side;
    int        halfmoves;
    
    int history_ply;
    FixedList<PositionUndo, 2046> history;
};