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
#include "types.h"
#include "network.h"
#include "bitboard.h"
#include "movelist.h"
#include "fixed_list.h"
#include "position_undo.h"

#include <array>
#include <memory>
#include <string_view>

class Position {
private:
    FixedList<PositionUndo, 2046> history;
    std::array<Piece, 64> pieces;
    std::array<std::uint64_t, 6> bitboards;
    std::array<std::uint64_t, 2> colors;
    Network network;

    ZobristKey key;
    std::uint64_t castle_rooks;
    int halfmoves, history_ply;
    Square ep_sq;
    Color side;

public:
    Position();

    // Set a fen string, does not attempt to validtae it
    void set_fen(std::string_view);

    /// Convert board to fen string
    std::string get_fen() const;

    // Generate all legal moves (both noisy and quiet)
    void generate_legal(Movelist &) const;

    // Generate all legal noisy moves
    void generate_noisy(Movelist &) const;

    // Generate all legal quiet moves
    void generate_quiet(Movelist &) const;

    // Check if a move is legal (only checks if performing a move will leave
    // king in check)
    bool is_legal(Move) const;

    // Check if a move is pseudolegal
    bool is_pseudolegal(Move) const;

    // Return a side relative static evaluation
    int static_evaluation();

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

    // Check if position is drawn by repetition, 50-move rule or insufficient
    // material
    bool drawn() const;

    // Check if side to move's king is under attack
    bool king_in_check() const;

    // Check if given square is attacked by the given side
    bool square_is_attacked(const Square, const Color) const;

    // Check if given square is attacked by the given side
    bool square_is_attacked(const Square, const Color, const std::uint64_t occupancy) const;

    std::uint64_t attackers_to_sq(const Square sq) const;

    // Previous move played
    Move previous_move() const {
        return history_ply > 0 ? history[history_ply - 1].move : NullMove;
    }

    // const_iterator to start of piece array
    auto begin() const {
        return pieces.begin();
    }

    // const_iterator to end of piece array
    auto end() const {
        return pieces.end();
    }

    // Add a piece on the board
    void add_piece(Square sq, Piece piece) {
        get_piece(sq) = piece;
        set_bit(get_bb(compute_piece_type(piece)), sq);
        set_bit(get_bb(compute_color(piece)), sq);
    }

    // Remove a piece from the board
    Piece remove_piece(Square sq) {
        Piece piece = get_piece(sq);
        flip_bit(get_bb(compute_piece_type(piece)), sq);
        flip_bit(get_bb(compute_color(piece)), sq);
        get_piece(sq) = PCE_NULL;

        return piece;
    }

    // Move piece from square a to square b
    void move_piece(Square a, Square b) {
        Piece piece    = get_piece(a);
        PieceType type = compute_piece_type(piece);
        Color color    = compute_color(piece);

        flip_bit(get_bb(type), a);
        flip_bit(get_bb(color), a);
        flip_bit(get_bb(type), b);
        flip_bit(get_bb(color), b);

        get_piece(b) = piece;
        get_piece(a) = PCE_NULL;
    }

    // Add a piece on the board with hash update
    void add_piece_hash(const Square sq, const Piece pce) {
        assert(is_ok(sq) && is_ok(pce));
        add_piece(sq, pce);
        zobrist_hash_piece(key, pce, sq);
    }

    // Remove a piece from the board  with hash update
    auto remove_piece_hash(const Square sq) {
        assert(is_ok(sq));
        const auto pce = remove_piece(sq);
        zobrist_hash_piece(key, pce, sq);
        return pce;
    }

    // Move piece from square a to square b with hash update
    void move_piece_hash(const Square a, const Square b) {
        assert(is_ok(a) && is_ok(b));
        const auto pce = get_piece(a);
        move_piece(a, b);
        zobrist_hash_piece(key, pce, a);
        zobrist_hash_piece(key, pce, b);
    }

    std::uint64_t &get_bb(PieceType pt) {
        return bitboards[pt];
    }

    std::uint64_t get_bb(PieceType pt) const {
        return bitboards[pt];
    }

    std::uint64_t &get_bb(Color color) {
        return colors[color];
    }

    std::uint64_t get_bb(Color color) const {
        return colors[color];
    }

    std::uint64_t get_bb(PieceType pt, Color color) const {
        return bitboards[pt] & colors[color];
    }

    std::uint64_t get_bb(Piece piece) const {
        return get_bb(compute_piece_type(piece), compute_color(piece));
    }

    std::uint64_t get_key() const {
        return key;
    }

    std::uint64_t get_bb() const {
        return colors[CLR_WHITE] | colors[CLR_BLACK];
    }

    std::uint64_t get_castle_bits() const {
        return castle_rooks;
    }

    Piece &get_piece(Square sq) {
        return pieces[sq];
    }

    Piece get_piece(Square sq) const {
        return pieces[sq];
    }

    Square get_ep() const {
        return ep_sq;
    }

    Color get_side() const {
        return side;
    }

    NetworkInput to_net_input() const;

    friend std::ostream &operator<<(std::ostream &, Position const &);
};