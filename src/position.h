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
    Position();

    void set_fen(std::string_view);

    std::string get_fen() const;

    void apply_move(Move);

    void apply_nullmove();

    void revert_move();

    void revert_nullmove();

    Move last_played();

    uint64_t& get_bb(PieceType pt)
    {
        return bitboards[pt];
    }

    uint64_t& get_bb(Color color)
    {
        return colors[color];
    }

    uint64_t get_bb(PieceType pt, Color color) const
    {
        return bitboards[pt] & colors[color];
    }

    uint64_t get_bb(Piece piece) const
    {
        return get_bb(type_of(piece), color_of(piece));
    }

    uint64_t get_bb() const
    {
        return colors[White] | colors[Black];
    }

    Piece& get_piece(Square sq) 
    {
        return pieces[sq];
    }
private:
    std::array<uint64_t,  6> bitboards;
    std::array<uint64_t,  2> colors;
    std::array<Piece   , 64> pieces; 

    ZobristKey key;
    Square     ep_sq;
    Color      side;
    int        halfmoves;
    
    int history_ply;
    FixedList<PositionUndo, 2046> history;
};