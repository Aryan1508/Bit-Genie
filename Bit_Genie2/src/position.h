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

  // Return the current hashed value
  // of the position
  ZobristKey hash() const;

private:
  // Reset all position attributes
  void reset(); 

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

  // The side which will play the next move
  Piece::Color side_to_play;
};