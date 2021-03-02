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

  // Create a new position with a Fen string 
  // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  Position(std::string_view);

  void parse_fen(std::string_view) const;

  ZobristKey hash() const;
private:
  // Reset ep_sq to Square::Bad
  void reset_enpassant();

  // Reset all position attributes
  void reset(); 

private:
  // 'pieces' answers the question "what piece stands on what square"
  // and uses btiboards internally to manage those pieces and perform 
  // functions like move-generation
  PieceManager pieces;

  // Is any en-passant possible in the position? ep_sq
  // holds the value of that Square
  Square ep_sq;

  // Zobrist-hash of the current position 
  ZobristKey key;

  // Total history of all previously-encountered
  // positions. Used for un-making moves
  // and detecting 3-fold repetitions
  PositionHistory history;
};