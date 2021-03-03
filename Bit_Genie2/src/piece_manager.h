#pragma once
#include <array>
#include "bitboard.h"
#include "piece.h"

// Class for managing different 
// bitboards of a position
class PieceManager 
{
  friend class Position;
public:
  PieceManager();

  // 0-init all arrays, fill squares with empty pieces
  void reset();

  // Set the en-passant square from a string-square
  // example sq -> 'e4'
  bool parse_fen_ep(std::string_view);

  // Initialize bitboards and squares from 
  // fen for pieces
  bool parse_fen_board(std::string_view);

  friend std::ostream& operator<<(std::ostream&, PieceManager const&);
private:
  // Add a piece (of fen format) to the given
  // square. Used in fen-parsing. 
  // Return value is to check if string is invalid
  bool add_piece(Square sq, const char piece);

  // Add a normal piece on
  // the board (update all arrays)
  void add_piece(const Square, const Piece);

  // Add a whole rank of pieces of a fen
  // Return value is to check if string is invalid
  bool add_rank(Square& counter, std::string_view);

  // Set ep to Square::bad
  void reset_ep();
public:
  // Total occupancy(Bitboards) of White / Black
  std::array<Bitboard, total_colors> colors;

  // Bitboards for each piece type
  std::array<Bitboard, total_pieces> pieces;

  // Array of pieces on each square
  std::array<Piece, total_squares> squares;

  // En-passant square of the board(if possible)
  Square ep_sq;
};