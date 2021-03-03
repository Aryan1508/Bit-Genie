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
public:
  Position();

  // Parse a fen 
  // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  bool set_fen(std::string_view);

  // Return the current hashed value
  // of the position
  ZobristKey hash() const;

  // Piece standing on given square
  Piece const& get_piece(const Square) const;

  Piece::Color player() const;
  CastleRights get_castle_rights() const;
  Square get_ep() const;

  friend std::ostream& operator<<(std::ostream&, Position const&);
private:
  // Reset all position attributes
  void reset(); 

  // set side_to_play from 'w' -> Piece::white or 'b' -> Piece::black
  bool parse_fen_side(std::string_view);

  // set halfmove_nb from fen 
  bool parse_fen_hmn(std::string_view);

  // (set to 0)
  void reset_halfmoves();
private:
  // 'pieces' answers the question "what piece stands on what square"
  // and uses btiboards internally to manage those pieces and perform 
  // functions like move-generation
  PieceManager pieces;

  // Zobrist-hash of the current position 
  ZobristKey key;

  // Total history of all previously-encountered
  // positions. Used for un-making moves
  // and detecting 3-fold repetitions
  PositionHistory history;

  // Manages castle rights 
  CastleRights castle_rights;

  // The side which will play the next move
  Piece::Color side_to_play;

  // Total number of half-moves played in the position
  int half_moves;
};