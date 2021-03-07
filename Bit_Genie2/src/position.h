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
  bool set_fen(std::string_view);

  uint64_t friend_bb() const;

  uint64_t enemy_bb() const;

  uint64_t total_occupancy() const;

  void apply_move(uint16_t);

  void revert_move();

  bool move_was_legal() const;

  bool move_is_legal(uint16_t);

  bool apply_move(std::string);

  uint64_t perft(int, bool = true);

  friend std::ostream& operator<<(std::ostream&, Position const&);
public:
  PieceManager pieces;

  CastleRights castle_rights;

  ZobristKey key;

  PositionHistory history;

private:
  void reset();

  bool parse_fen_side(std::string_view);

  bool parse_fen_hmn(std::string_view);

  void reset_halfmoves();

  void reset_ep();

  bool parse_fen_ep(std::string_view);

  inline void switch_players()
  {
    side = !side;
  }

  Piece apply_normal_move(uint16_t);
  
  Piece apply_enpassant(uint16_t);

  Piece apply_castle(uint16_t);

  Piece apply_promotion(uint16_t);
  void revert_normal_move(uint16_t, Piece);

  void revert_enpassant(uint16_t, Piece);

  void revert_castle(uint16_t);

  void revert_promotion(uint16_t, Piece);

  void update_ep(Square from, Square to);
public:
  Color side;
  int half_moves;
  Square ep_sq;
};