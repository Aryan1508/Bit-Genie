#pragma once
#include <array>
#include "bitboard.h"
#include "castle_rights.h"
#include "piece_manager.h"
#include "piece.h"
#include "position_history.h"
#include <string_view>
#include "Square.h"
#include "zobrist.h"
#include <vector>

class Position 
{
  friend class PositionHistory;
public:
  Position();

  // Parse a fen 
  // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  bool set_fen(std::string_view);

  // Return the current player's color
  Color player() const;

  // Return the current state of the en-passant 
  Square get_ep() const;

  // uint64_t-occupancy of the current player
  uint64_t friend_bb() const;

  // uint64_t-occupancy of the enemy player
  uint64_t enemy_bb() const;

  // uint64_t-occupancy of both the players
  uint64_t total_occupancy() const;

  // Perform a move (class Move) on the position
  void apply_move(uint16_t);

  // Undo the latest move applied
  void revert_move();

  int get_halfmoves() const;

   // Check if a pseudo-legal is legal 
  // i.e It shouldn't leave our own king in check
  bool move_was_legal() const;

  void error_check_bb() const;

  bool move_is_legal(uint16_t);

  uint64_t perft(int, bool = true);

  friend std::ostream& operator<<(std::ostream&, Position const&);
public:
  // Manage board-pieces
  PieceManager pieces;

  // Manage castle rights
  CastleRights castle_rights;

  // Zobrist-hash of the current position 
  ZobristKey key;

  // Total history of all previously-encountered
  // positions. Used for un-making moves
  // and detecting 3-fold repetitions
  PositionHistory history;
private:
  // Reset all position attributes
  void reset(); 

  // set side_to_play from 'w' -> Color::white or 'b' -> Color::black
  bool parse_fen_side(std::string_view);

  // set halfmove_nb from fen 
  bool parse_fen_hmn(std::string_view);

  // (set to 0)
  void reset_halfmoves();

  // set to Square::bad
  void reset_ep();

  // Parse the fen square for ep into ep_sq
  bool parse_fen_ep(std::string_view);

  // pass the turn to the opponent
  inline void switch_players()
  {
    side_to_play = !side_to_play;
  }

  // Add the given piece on the given square
  // and update the Zobrist key
  void add_piece(Square, Piece);
  
  // Remove the piece standing on the given square
  // and update the Zobrist key
  Piece clear_sq(Square);

  // Perform a normal(not castle, enpassant or promotion) move
  Piece apply_normal_move(uint16_t);

  Piece apply_enpassant(uint16_t);

  // Perform a normal(not castle, enpassant or promotion) move
  void revert_normal_move(uint16_t, Piece);

  void revert_enpassant(uint16_t, Piece);

  void update_ep(Square from, Square to);
public:
  // The side which will play the next move
  Color side_to_play;

  // Total number of half-moves played in the position
  int half_moves;

  // En-passant square of the board(if possible)
  Square ep_sq;
};