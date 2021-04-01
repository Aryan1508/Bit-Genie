/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "attacks.h"
#include <iomanip>
#include "move.h"
#include "position.h"
#include "stringparse.h"
#include "movegen.h"
#include <vector>
#include <algorithm>

Position::Position()
{
	set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Position::reset()
{
	key.reset();
	pieces.reset();
	castle_rights.reset();
	reset_halfmoves();
	side = White;
}

void Position::reset_halfmoves()
{
	half_moves = 0;
}

uint64_t Position::friend_bb() const
{
	return pieces.get_occupancy(side);
}

uint64_t Position::enemy_bb() const
{
	return pieces.get_occupancy(!side);
}

uint64_t Position::total_occupancy() const
{
	return friend_bb() | enemy_bb();
}

bool Position::parse_fen_side(std::string_view label)
{
	if (label == "w")
		side = White;

	else if (label == "b")
		side = Black;

	else
		return false;

	return true;
}

bool Position::parse_fen_hmn(std::string_view fen)
{
	reset_halfmoves();
	if (!string_is_number(fen))
	{
		return false;
	}
	half_moves = std::stoi(fen.data());
	return true;
}

bool Position::set_fen(std::string_view fen)
{
	Position temp = *this;
	reset();

	
	std::vector<std::string> parts = split_string(fen, ' ');

	if (!(parts.size() >=  4 && parts.size() <= 6))
		return false;


	bool valid = pieces.parse_fen_board(parts[0]) && parse_fen_side(parts[1])
		&& castle_rights.parse_fen(parts[2]) && parse_fen_ep(parts[3]);

	if (parts.size() == 5)
		valid = valid && parse_fen_hmn(parts[4]);
	else
		half_moves = 0;

	if (!valid)
	{
		*this = temp;
		return false;
	}

	key.generate(*this);
	history.total = 0;
	return valid;
}

void Position::reset_ep()
{
	ep_sq = Square::bad_sq;
}

bool Position::parse_fen_ep(std::string_view sq)
{
	reset_ep();
	if (sq == "-")
		return true;

	if (!is_valid_sq(sq))
		return false;

	ep_sq = to_sq(sq);
	return true;
}

std::ostream& operator<<(std::ostream& o, Position const& position)
{
	o << position.pieces << "\n";
	o << "\nside to play : " << position.side;
	o << "\ncastle rights: " << position.castle_rights;
	o << "\nen-passant sq: " << position.ep_sq;
	o << "\nhalf-moves   : " << position.half_moves;
	o << "\nzobrist-key  : " << position.key;
	o << "\nhistory-ply  : " << position.history.total;
	return o << '\n';
}

static inline bool is_double_push(Square from, Square to, PieceType moving)
{
	return (from ^ to) == 16;
}

void Position::update_ep(Square from, Square to)
{
	uint64_t potential = BitMask::pawn_attacks[side][to ^ 8];
	uint64_t enemy_pawns = pieces.get_piece_bb<Pawn>(!side);

	if (potential & enemy_pawns)
	{
		ep_sq = to_sq(to ^ 8);
		key.hash_ep(to_sq(to ^ 8));
	}
}

Piece Position::apply_enpassant(Move move)
{
	reset_halfmoves();

	Square from = move_from(move);
	Square to = move_to(move);
	Square ep = to_sq(to_int(move_to(move)) ^ 8);

	Piece from_pce = pieces.squares[from];
	Piece captured = pieces.squares[ep];

	pieces.bitboards[type_of(from_pce)] ^= (1ull << from) | (1ull << to);
	pieces.colors[color_of(from_pce)] ^= (1ull << from) | (1ull << to);

	pieces.bitboards[type_of(captured)] ^= (1ull << ep);
	pieces.colors[color_of(captured)] ^= (1ull << ep);

	pieces.squares[to] = from_pce;
	pieces.squares[from] = Empty;
	pieces.squares[ep] = Empty;

	key.hash_piece(from, from_pce);
	key.hash_piece(to, from_pce);
	key.hash_piece(ep, captured);

	return captured;
}

void Position::revert_normal_move(Move move, Piece captured)
{
	Square from = move_from(move);
	Square to = move_to(move);

	Piece from_pce = pieces.squares[to];

	pieces.bitboards[type_of(from_pce)] ^= ((1ull << from) | (1ull << to));
	pieces.colors[color_of(from_pce)] ^= ((1ull << from) | (1ull << to));

	if (!(captured == Empty)) {
		pieces.bitboards[type_of(captured)] ^= (1ull << to);
		pieces.colors[color_of(captured)] ^= (1ull << to);
	}

	pieces.squares[from] = pieces.squares[to];
	pieces.squares[to] = captured;
}

void Position::revert_enpassant(Move move, Piece captured)
{
	Square from = move_from(move);
	Square to = move_to(move);

	Square ep = to_sq(to_int(move_to(move)) ^ 8);
	uint64_t ep_bb = 1ull << to_int(ep);
	Piece from_pce = pieces.squares[to];

	pieces.bitboards[type_of(from_pce)] ^= ((1ull << from) | (1ull << to));
	pieces.colors[color_of(from_pce)] ^= ((1ull << from) | (1ull << to));

	pieces.bitboards[type_of(captured)] ^= ep_bb;
	pieces.colors[color_of(captured)] ^= ep_bb;

	pieces.squares[ep] = captured;
	pieces.squares[from] = pieces.squares[to];
	pieces.squares[to] = Empty;
}

bool Position::move_was_legal() const
{
	Square our_king = get_lsb(pieces.get_piece_bb(make_piece(King, !side)));
	return !Attacks::square_attacked(*this, our_king, side);
}

static inline uint64_t legal_move_count(Position& position)
{
	MoveGenerator gen;
	gen.generate(position);
	return gen.movelist.size();
}

void Position::perft(int depth, uint64_t& nodes, bool root)
{
	if (depth == 1)
	{
		nodes += legal_move_count(*this);
		return;
	}

	MoveGenerator gen;
	gen.generate(*this);

	for (Move m : gen.movelist)
	{
		apply_move(m);
		uint64_t old = nodes;
		perft(depth - 1, nodes, false);
		revert_move();

		if (root)
			std::cout << print_move(m) << ": " << nodes - old << std::endl;
	}
}

bool Position::move_is_legal(Move move)
{
	if (!move)
		return false;

	Square from = move_from(move);
	Square to = move_to(move);

	if (move_flag(move) == MoveFlag::normal || move_flag(move) == MoveFlag::promotion)
	{
		if (pieces.squares[from] == wKing || pieces.squares[from] == bKing) // Normal king moves
		{
			uint64_t occupancy = total_occupancy() ^ (1ull << from);
			return !Attacks::square_attacked(*this, to, !side, occupancy);
		}

		else // Normal non-king moves
		{
			uint64_t occupancy = total_occupancy() ^ (1ull << from) ^ (1ull << to);
			uint64_t enemy = enemy_bb();

			Piece captured = pieces.squares[to];

			if (captured != Empty)
			{
				occupancy ^= (1ull << to);
				enemy ^= (1ull << to);
			}

			uint64_t pawns = pieces.bitboards[Pawn] & enemy;
			uint64_t knights = pieces.bitboards[Knight] & enemy;
			uint64_t bishops = pieces.bitboards[Bishop] & enemy;
			uint64_t rooks = pieces.bitboards[Rook] & enemy;
			uint64_t queens = pieces.bitboards[Queen] & enemy;

			Square king = get_lsb(pieces.bitboards[King] & friend_bb());

			bishops |= queens;
			rooks |= queens;

			return !((BitMask::pawn_attacks[side][king] & pawns)
					 || (Attacks::bishop(king, occupancy) & bishops)
					 || (Attacks::rook(king, occupancy) & rooks)
					 || (Attacks::knight(king) & knights));
		}
	}

	else if (move_flag(move) == MoveFlag::castle)
	{
		return !Attacks::square_attacked(*this, to, !side);
	}

	else
	{
		Square ep = to_sq(to ^ 8);
		uint64_t occupancy = total_occupancy() ^ (1ull << from) ^ (1ull << to) ^ (1ull << ep);
		uint64_t enemy = enemy_bb() ^ (1ull << ep);

		uint64_t pawns = pieces.bitboards[Pawn] & enemy;
		uint64_t knights = pieces.bitboards[Knight] & enemy;
		uint64_t bishops = pieces.bitboards[Bishop] & enemy;
		uint64_t rooks = pieces.bitboards[Rook] & enemy;
		uint64_t queens = pieces.bitboards[Queen] & enemy;

		Square king = get_lsb(pieces.bitboards[King] & friend_bb());

		bishops |= queens;
		rooks |= queens;

		return !((BitMask::pawn_attacks[side][king] & pawns)
				 || (Attacks::bishop(king, occupancy) & bishops)
				 || (Attacks::rook(king, occupancy) & rooks)
				 || (Attacks::knight(king) & knights));
	}

	return true;
}

Piece Position::apply_castle(Move move)
{
	auto old_castle = castle_rights;
	Square from = move_from(move);
	Square to = move_to(move);
	Square rook_from = bad_sq, rook_to = bad_sq;
	Color col = White;

	switch (to)
	{
		case C1:
			rook_from = A1;
			rook_to = D1;
			col = White;
			break;

		case G1:
			rook_from = H1;
			rook_to = F1;
			col = White;
			break;

		case C8:
			rook_from = A8;
			rook_to = D8;
			col = Black;
			break;

		case G8:
			rook_from = H8;
			rook_to = F8;
			col = Black;
			break;

		default:
			assert(false);
			break;
	}

	PieceType rook = type_of(pieces.squares[rook_from]);
	PieceType king = type_of(pieces.squares[from]);

	pieces.bitboards[king] ^= (1ull << from) ^ (1ull << to);
	pieces.bitboards[rook] ^= (1ull << rook_from) ^ (1ull << rook_to);
	pieces.colors[col] ^= (1ull << from) ^ (1ull << to) ^ (1ull << rook_from) ^ (1ull << rook_to);

	pieces.squares[to] = pieces.squares[from];
	pieces.squares[from] = Empty;

	pieces.squares[rook_to] = pieces.squares[rook_from];
	pieces.squares[rook_from] = Empty;

	castle_rights.update(move);

	key.hash_piece(from, make_piece(king, col));
	key.hash_piece(to, make_piece(king, col));
	key.hash_piece(rook_from, make_piece(rook, col));
	key.hash_piece(rook_to, make_piece(rook, col));
	key.hash_castle(old_castle, castle_rights);

	return Empty;
}

void Position::revert_castle(Move move)
{
	Square from = move_from(move);
	Square to = move_to(move);
	Square rook_from = bad_sq, rook_to = bad_sq;
	Color col = White;

	switch (to)
	{
		case C1:
			rook_from = A1;
			rook_to = D1;
			col = White;
			break;

		case G1:
			rook_from = H1;
			rook_to = F1;
			col = White;
			break;

		case C8:
			rook_from = A8;
			rook_to = D8;
			col = Black;
			break;

		case G8:
			rook_from = H8;
			rook_to = F8;
			col = Black;
			break;

		default:
			assert(false);
			break;
	}

	PieceType rook = type_of(pieces.squares[rook_to]);
	PieceType king = type_of(pieces.squares[to]);

	pieces.bitboards[king] ^= (1ull << from) ^ (1ull << to);
	pieces.bitboards[rook] ^= (1ull << rook_from) ^ (1ull << rook_to);
	pieces.colors[col] ^= (1ull << from) ^ (1ull << to) ^ (1ull << rook_from) ^ (1ull << rook_to);

	pieces.squares[from] = pieces.squares[to];
	pieces.squares[to] = Empty;

	pieces.squares[rook_from] = pieces.squares[rook_to];
	pieces.squares[rook_to] = Empty;
}

Piece Position::apply_normal_move(Move move)
{
	CastleRights old_castle = castle_rights;
	Square from = move_from(move);
	Square to = move_to(move);
	Piece from_pce = pieces.squares[from];

	PieceType from_pce_t = type_of(from_pce);
	Color from_pce_c = color_of(from_pce);

	Piece captured = pieces.squares[to];

	if (type_of(from_pce) == Pawn)
	{
		reset_halfmoves();

		if (is_double_push(from, to, from_pce_t))
			update_ep(from, to);
	}

	if (captured != Empty)
	{
		reset_halfmoves();

		pieces.bitboards[type_of(captured)] ^= (1ull << to);
		pieces.colors[color_of(captured)] ^= (1ull << to);
		key.hash_piece(to, captured);
	}

	pieces.bitboards[from_pce_t] ^= ((1ull << from) | (1ull << to));
	pieces.colors[from_pce_c] ^= ((1ull << from) | (1ull << to));

	pieces.squares[to] = pieces.squares[from];
	pieces.squares[from] = Empty;

	castle_rights.update(move);

	key.hash_piece(from, from_pce);
	key.hash_piece(to, from_pce);
	key.hash_castle(old_castle, castle_rights);

	return captured;
}

Piece Position::apply_promotion(Move move)
{
	reset_halfmoves();

	CastleRights old_castle = castle_rights;
	Square from = move_from(move);
	Square to = move_to(move);
	Piece from_pce = pieces.squares[from];

	PieceType from_pce_t = type_of(from_pce);
	Color from_pce_c = color_of(from_pce);

	PieceType prom_pce = move_promoted(move);

	Piece captured = pieces.squares[to];

	if (captured != Empty)
	{
		pieces.bitboards[type_of(captured)] ^= (1ull << to);
		pieces.colors[color_of(captured)] ^= (1ull << to);
		key.hash_piece(to, captured);
	}

	pieces.bitboards[from_pce_t] ^= (1ull << from);
	pieces.bitboards[prom_pce] ^= (1ull << to);
	pieces.colors[from_pce_c] ^= ((1ull << from) | (1ull << to));

	pieces.squares[from] = Empty;
	pieces.squares[to] = make_piece(prom_pce, from_pce_c);

	castle_rights.update(move);

	key.hash_piece(from, from_pce);
	key.hash_piece(to, make_piece(prom_pce, from_pce_c));
	key.hash_castle(old_castle, castle_rights);

	return captured;
}

void Position::revert_promotion(Move move, Piece captured)
{
	Square from = move_from(move);
	Square to = move_to(move);

	Piece original = make_piece(PieceType::Pawn, !side);
	Piece prom_pce = pieces.squares[to];

	pieces.bitboards[type_of(original)] ^= (1ull << from);
	pieces.colors[color_of(original)] ^= (1ull << from) ^ (1ull << to);

	pieces.bitboards[type_of(prom_pce)] ^= (1ull << to);

	if (captured != Empty) {
		pieces.bitboards[type_of(captured)] ^= (1ull << to);
		pieces.colors[color_of(captured)] ^= (1ull << to);
	}

	pieces.squares[from] = original;
	pieces.squares[to] = captured;
}

void Position::apply_move(Move move)
{
	auto& undo = save(move);

	if (ep_sq != Square::bad_sq)
		key.hash_ep(ep_sq);

	reset_ep();
	half_moves++;

	MoveFlag type = move_flag(move);

	if (type == MoveFlag::normal)
		undo.captured = apply_normal_move(move);

	else if (type == MoveFlag::enpassant)
		undo.captured = apply_enpassant(move);

	else if (type == MoveFlag::castle)
		undo.captured = apply_castle(move);
	else
		undo.captured = apply_promotion(move);

	key.hash_side();
	switch_players();
}

void Position::revert_move()
{
	Move move = NullMove;
	Piece captured = Empty;

	restore(move, captured);

	MoveFlag type = move_flag(move);

	if (type == MoveFlag::normal)
		revert_normal_move(move, captured);

	else if (type == MoveFlag::enpassant)
		revert_enpassant(move, captured);

	else if (type == MoveFlag::castle)
		revert_castle(move);

	else
		revert_promotion(move, captured);

	switch_players();
}

bool Position::apply_move(std::string move)
{
	MoveGenerator gen;
	gen.generate(*this);

	for (Move m : gen.movelist)
	{
		if (print_move(m) == move)
		{
			apply_move(m);
			return true;
		}
	}

	return false;
}

bool Position::king_in_check() const
{
	uint64_t king = pieces.get_piece_bb<King>(side);
	return Attacks::square_attacked(*this, get_lsb(king), !side);
}

bool Position::move_exists(Move move)
{
	MoveGenerator gen;
	gen.generate(*this);
	return gen.movelist.find(move);
}

bool Position::move_is_pseudolegal(Move move)
{
	if (!move_without_score(move))
		return false;

	Square from    = move_from(move);
	Square to      = move_to(move);
	MoveFlag flag  = move_flag(move);
	Piece moving   = pieces.squares[from];
	Piece captured = pieces.squares[to];

	// Make sure from and to squares are valid
	if (!is_ok(from) || !is_ok(to))
		return false;

	// Moving piece can't be empty
	if (moving == Empty)
		return false;

	// Color of the piece has to be the current 
	// side to play
	if (color_of(moving) != side)
		return false;

	// Cannot capture our own piece
	if (captured != Empty && color_of(captured) == side)
		return false;

	if (flag == MoveFlag::castle)
	{
		MoveGenerator<false> gen;
		gen.generate_castle(*this);
		return !king_in_check() && gen.movelist.find(move);
	}

	// Validating pawn moves
	if (moving == wPawn || moving == bPawn)
	{
		Rank       start_rank = side == White ? Rank::two : Rank::seven;
	    Direction  forward = side == White ? Direction::north : Direction::south;
		Square     forward_sq = from + forward;
		
		// Normal and promotions
		if (flag == MoveFlag::normal || flag == MoveFlag::promotion)
		{
			// Pushes
			if (pieces.squares[to] == Empty)
			{
				Direction double_push = forward + forward;
				Square    double_push_sq = from + double_push;

				// Single push
				if (to == forward_sq)
					return true;

				// Double pushes 
				return (to == double_push_sq && pieces.squares[forward_sq] == Empty && rank_of(from) == start_rank);
			}
			// Pawn captures
			// Already confirmed that captured piece isn't ours
			else
			{
				return test_bit(to, BitMask::pawn_attacks[side][from]);
			}
		}

		// Enpassant moves
		else
		{
			if (to != ep_sq)
				return false;

			if (!((BitMask::neighbor_files[ep_sq]) & (1ull << from)))
				return false;

			captured = pieces.squares[to ^ 8];

			if (captured == Empty || color_of(captured) == side)
				return false;

			Square down_right = to - forward + Direction::east;
			Square down_left  = to - forward + Direction::west;

			return (from == down_left || from == down_right);
		}
	}

	// Validating knights, kings and sliding pieces
	else 
	{
		// Other pieces don't have special moves
		// except for king castles, but we already checked that
		if (flag != MoveFlag::normal)
			return false; 

		uint64_t attacks = Attacks::generate(type_of(moving), from, total_occupancy());
		return test_bit(to, attacks);
	}

	return false;
}

void Position::apply_move(Move move, int& ply)
{
	ply++;
	apply_move(move);
}

void Position::revert_move(int& ply)
{
	ply--;
	revert_move();
}

PositionHistory::Undo& Position::save(Move move)
{
	auto& undo = history.current();

	undo.move   = move;
	undo.key    = key;
	undo.ep_sq  = ep_sq;
	undo.castle = castle_rights;
	undo.half_moves = half_moves;

	history.total++;
	
	return undo;
}

void Position::restore()
{
	auto& undo = history.previous();

	key           = undo.key;
	ep_sq		  = undo.ep_sq;
	castle_rights = undo.castle;
	half_moves	  = undo.half_moves;

	history.total--;
}

void Position::restore(Move& move, Piece& captured)
{
	auto& undo = history.previous();

	key = undo.key;
	ep_sq = undo.ep_sq;
	castle_rights = undo.castle;
	half_moves = undo.half_moves;
	move = undo.move;
	captured = undo.captured;

	history.total--;
}

void Position::apply_null_move(int& ply)
{
	ply++;
	save();

	if (ep_sq != bad_sq)
		key.hash_ep(ep_sq);
	key.hash_side();
	switch_players();
}

void Position::revert_null_move(int& ply)
{
	ply--;
	restore();
	switch_players();
}

bool Position::should_do_null() const
{
	return popcount64(total_occupancy()) >= 4;
}