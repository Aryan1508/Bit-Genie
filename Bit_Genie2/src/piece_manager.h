#pragma once
#include <array>
#include "bitboard.h"
#include "piece.h"
#include <vector>

class PieceManager 
{
  friend class Position;
public:
  PieceManager();
  void reset();
  bool parse_fen_board(std::string_view);
  uint64_t get_piece_bb(Piece) const;
  uint64_t get_occupancy(Color) const;
  Piece const& get_piece(const Square) const;

  template<PieceType type>
  inline uint64_t get_piece_bb(Color color) const
  {
    return bitboards[to_int(type)] & colors[to_int(color)];
  }

  friend std::ostream& operator<<(std::ostream&, PieceManager const&);

private:
  bool add_piece(Square sq, char piece);
  void add_piece(Square, Piece);
  bool add_rank(Square& counter, std::string_view);

public:
  std::array<uint64_t, total_colors> colors;
  std::array<uint64_t, total_pieces> bitboards;
  std::array<Piece, total_squares> squares;
};