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

class Position 
{
  friend class PositionHistory;
public:
  Position();

  // Parse a fen 
  // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  bool set_fen(std::string_view);

  // Return the current player's color
  Piece::Color player() const;

  // Return the current state of the en-passant 
  Square get_ep() const;

  // bitboard-occupancy of the current player
  Bitboard friend_bb() const;

  // bitboard-occupancy of the enemy player
  Bitboard enemy_bb() const;

  // bitboard-occupancy of both the players
  Bitboard total_occupancy() const;

  // Perform a move (class Move) on the position
  void apply_move(Move);

  // Undo the latest move applied
  void revert_move(Move);

  int get_halfmoves() const;

   // Check if a pseudo-legal is legal 
  // i.e It shouldn't leave our own king in check
  bool move_was_legal(Move) const;

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

  // set side_to_play from 'w' -> Piece::white or 'b' -> Piece::black
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
    side_to_play = switch_color(side_to_play);
  }

  // Add the given piece on the given square
  // and update the Zobrist key
  void add_piece(Square, Piece);
  
  // Remove the piece standing on the given square
  // and update the Zobrist key
  Piece clear_sq(Square);

  // Perform a normal(not castle, enpassant or promotion) move
  void apply_normal_move(Move);

  Piece apply_enpassant(Move);

  // Perform a normal(not castle, enpassant or promotion) move
  void revert_normal_move(Move, Piece);

  void revert_enpassant(Move, Piece);

  void update_ep(Square from, Square to);
  
public:
  // The side which will play the next move
  Piece::Color side_to_play;

  // Total number of half-moves played in the position
  int half_moves;

  // En-passant square of the board(if possible)
  Square ep_sq;
};