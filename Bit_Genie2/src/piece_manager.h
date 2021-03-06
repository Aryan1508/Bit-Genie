#pragma once
#include <array>
#include "bitboard.h"
#include "piece.h"
#include <vector>

// Class for managing different 
// bitboards of a position
class PieceManager 
{
  friend class Position;
public:
  PieceManager();

  // 0-init all arrays, fill squares with empty pieces
  void reset();

  // Initialize bitboards and squares from 
  // fen for pieces
  bool parse_fen_board(std::string_view);

  // uint64_t of all positions of the current piece
  uint64_t get_piece_bb(Piece) const;

  // Total occupancy of a color
  uint64_t get_occupancy(Piece::Color) const;

  // Return the piece standing on the given square
  Piece const& get_piece(const Square) const;

  // Return the square of which the king of the given
  // color is situated in 
  Square get_king_sq(Piece::Color) const;

  friend std::ostream& operator<<(std::ostream&, PieceManager const&);
private:
  // Add a piece (of fen format) to the given
  // square. Used in fen-parsing. 
  // Return value is to check if string is invalid
  bool add_piece(Square sq, char piece);

  // Add a normal piece on
  // the board (update all arrays)
  void add_piece(Square, Piece);

  void add_piece(Square, uint64_t, Piece);

  Piece clear_sq(Square);

  // Add a whole rank of pieces of a fen
  // Return value is to check if string is invalid
  bool add_rank(Square& counter, std::string_view);
public:
  // Total occupancy(bitboards) of White / Black
  std::array<uint64_t, total_colors> colors;

  // bitboards for each piece type
  std::array<uint64_t, total_pieces> bitboards;

  // Array of pieces on each square
  std::array<Piece, total_squares> squares;
};