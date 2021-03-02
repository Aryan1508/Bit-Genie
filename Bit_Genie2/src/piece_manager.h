#pragma once
#include <array>
#include "bitboard.h"
#include "piece.h"

// Class for managing different 
// bitboards of a position
class PieceManager 
{
public:
  PieceManager();

  // 0-init all arrays, fill squares with empty pieces
  void reset();
private:
  // Total occupancy(Bitboards) of White / Black
  std::array<Bitboard, total_colors> colors;

  // Bitboards for each piece type
  std::array<Bitboard, total_pieces> pieces;

  // Array of pieces on each square
  std::array<Piece, total_squares> squares;

  // En-passant square of the board(if possible)
  Square ep_sq;
};