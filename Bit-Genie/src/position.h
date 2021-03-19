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

	void apply_move(Move);

	void revert_move();

	void apply_null_move(int& search_ply);

	void revert_null_move(int& search_ply);

	void apply_move(Move, int& search_ply);

	void revert_move(int& search_ply);

	bool move_was_legal() const;

	bool move_is_legal(Move);

	bool move_is_pseudolegal(Move);

	bool move_exists(Move);

	bool apply_move(std::string);

	void perft(int, uint64_t&, bool = true);

	bool king_in_check() const;

	bool should_apply_null() const;

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

	Piece& save(Move = NullMove);

	void restore();

	void restore(Move&, Piece&);

	inline void switch_players()
	{
		side = !side;
	}

	Piece apply_normal_move(Move);

	Piece apply_enpassant(Move);

	Piece apply_castle(Move);

	Piece apply_promotion(Move);

	void revert_normal_move(Move, Piece);

	void revert_enpassant(Move, Piece);

	void revert_castle(Move);

	void revert_promotion(Move, Piece);

	void update_ep(Square from, Square to);
public:
	Color side;
	int half_moves;
	Square ep_sq;
};