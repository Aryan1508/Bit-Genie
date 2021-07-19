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
#include "bitboard.h"
#include "fixed_list.h"
#include "position_undo.h"

#include <string_view>

class Position
{
private:
    std::array<uint64_t,  6> bitboards;
    std::array<uint64_t,  2> colors;
    std::array<Piece   , 64> pieces; 

    ZobristKey key;
    Square     ep_sq;
    Color      side;
    uint64_t   castle_rooks;
    
    int halfmoves;
    int history_ply;
    FixedList<PositionUndo, 2046> history;

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

    // Check if a move is legal (only checks if performing a move will leave king in check)
    bool is_legal(Move) const;

    // Check if a move is pseudolegal
    bool is_pseudolegal(Move) const;

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

    // Check if position is drawn by repetition, 50-move rule or insufficient material
    bool drawn() const;

    // Check if side to move's king is under attack 
    bool king_in_check() const;

    // Add a piece on the board 
    void add_piece(Square sq, Piece piece)
    {
        get_piece(sq) = piece;
        set_bit(get_bb(type_of(piece)), sq);
        set_bit(get_bb(color_of(piece)), sq);
    }

    // Remove a piece from the board 
    Piece remove_piece(Square sq)
    {
        Piece piece = get_piece(sq);
        flip_bit(get_bb(type_of(piece)), sq);
        flip_bit(get_bb(color_of(piece)), sq);
        get_piece(sq) = Piece::Empty;
        return piece;
    }

    // Move piece from square a to square b
    void move_piece(Square a, Square b)
    {
        Piece piece = get_piece(a);
        PieceType type = type_of(piece);
        Color    color = color_of(piece);

        flip_bit(get_bb(type) , a);
        flip_bit(get_bb(color), a);
        flip_bit(get_bb(type) , b);
        flip_bit(get_bb(color), b);

        get_piece(b) = piece;
        get_piece(a) = Piece::Empty;
    }   

    // Add a piece on the board with hash update
    void add_piece_hash(Square sq, Piece piece)
    {
        add_piece(sq, piece);
        key.hash_piece(sq, piece);
    }

    // Remove a piece from the board  with hash update
    Piece remove_piece_hash(Square sq)
    {
        Piece piece = remove_piece(sq);
        key.hash_piece(sq, piece);
        return piece;
    }

    // Move piece from square a to square b with hash update
    void move_piece_hash(Square a, Square b)
    {
        Piece piece = get_piece(a);
        move_piece(a, b);
        key.hash_piece(a, piece);
        key.hash_piece(b, piece);
    }   

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

    uint64_t get_castle_rooks() const 
    {
        return castle_rooks;
    }

    Piece& get_piece(Square sq) 
    {
        return pieces[sq];
    }

    Piece get_piece(Square sq) const 
    {
        return pieces[sq];
    }

    Square get_ep() const 
    {
        return ep_sq;
    }

    Color get_side() const 
    {
        return side;
    }

    friend std::ostream& operator<<(std::ostream&, Position const&);
};