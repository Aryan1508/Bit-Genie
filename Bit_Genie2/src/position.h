#pragma once
#include "bitboard.h"
#include "piece.h"
#include "Square.h"
#include "zobrist.h"
#include <array>
#include <string_view>

class Position {
public:
  Position();

  // Create a new position with a Fen string 
  // https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation
  Position(std::string_view);

  ZobristKey hash() const;
private:
  void reset_bitboards();
  void reset_squares();
  void reset_enpassant();
  void reset(); 

private:
  // All pieces(with color) on the board indexed by Square
  std::array<Piece, total_squares> squares;

  // Bitboards of pieces (pawn, knight etc) indexed by Piece::Type
  std::array<Bitboard, total_pieces> bitboards;

  // Total occupancy a each color, indexed by Piece::Color 
  std::array<Bitboard, total_colors> colors;

  // Is any en-passant possible in the position? ep_sq
  // holds the value of that Square
  Square ep_sq;

  // Zobrist-hash of the current position 
  ZobristKey key;
};