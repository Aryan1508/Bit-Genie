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
#include "movelist.h"
#include "fixed_list.h"
#include "position_undo.h"

#include <string_view>

class Position
{
public:
    Position();

    // Set a fen string, does not attempt to validtae it
    void set_fen(std::string_view);

    /// Convert board to fen string
    std::string get_fen() const;

    // Generate all legal moves (both noisy and quiet)
    void generate_legal(Movelist&) const;
    
    // Generate all legal noisy moves
    void generate_noisy(Movelist&) const;

    // Generate all legal quiet moves
    void generate_quiet(Movelist&) const;

    // Perform a move over the board, UB if move is not legal
    void apply_move(Move);
    
    // Convert UCI move format and perform
    void apply_move(std::string_view);

    // Perform a null move 
    void apply_nullmove();

    // Revert the last move made, might crash if called on root
    void revert_move();

    // Revert a null move made
    void revert_nullmove();

    // Add a piece on the board, Assumes that 
    // given square and piece are valid and the no other piece exists 
    void add_piece(Square, Piece);

    // Check if position is drawn by repetition, 50-move rule or insufficient material
    bool drawn() const;

    // Check if side to move's king is under attack 
    bool king_in_check() const;


    uint64_t& get_bb(PieceType pt)
    {
        return bitboards[pt];
    }

    uint64_t get_bb(PieceType pt) const
    {
        return bitboards[pt];
    }

    uint64_t& get_bb(Color color)
    {
        return colors[color];
    }

    uint64_t get_bb(Color color) const  
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

    uint64_t get_key() const 
    {
        return key.data();
    }

    uint64_t get_bb() const
    {
        return colors[White] | colors[Black];
    }

    Piece& get_piece(Square sq) 
    {
        return pieces[sq];
    }

    Piece get_piece(Square sq) const 
    {
        return pieces[sq];
    }

    Color get_side() const 
    {
        return side;
    }

    friend std::ostream& operator<<(std::ostream&, Position const&);
private:
    std::array<uint64_t,  6> bitboards;
    std::array<uint64_t,  2> colors;
    std::array<Piece   , 64> pieces; 

    ZobristKey key;
    Square     ep_sq;
    Color      side;
    int        halfmoves;
    uint64_t   castle_rooks;
    
    int history_ply;
    FixedList<PositionUndo, 2046> history;
};