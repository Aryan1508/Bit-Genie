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
#pragma once
#include "misc.h"
#include <string_view>

// Wrapper around a bitboard to manage the castling rights of a position.
// It works by using a single bitboard that has all the rooks that can castle
// I.e if black can castle short then the G8 Square on the bitboard would be set
class CastleRights
{
public:
    CastleRights();

    // Reset to 0
    void reset();

    // parse a castle fen ('KQkq')
    bool parse_fen(std::string_view);

    // Return the rooks available for castling of a specific color.
    uint64_t get_rooks(Color) const;

    // Update the castle rights using a move
    // If a move includes moving the rook, then that side loses
    // its ability to castle using that rook.  If the king moves
    // then that side cannot castle at all
    void update(Move);

    uint64_t data() const { return rooks; }

    // Check if the squares needed for castling are not occupied by any piece
    static bool castle_path_is_clear(Square rook, uint64_t);

    // Return that squares that would be covered if the given rook was to castle
    static uint64_t get_castle_path(Square rook);

    // print out a fen representation of the current castle rights
    friend std::ostream &operator<<(std::ostream &, const CastleRights);

private:
    bool set(const char);

private:
    uint64_t rooks;
};
