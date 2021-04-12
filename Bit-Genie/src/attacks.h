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
#pragma once
#include "position.h"
#include "magicmoves.hpp"

namespace Attacks
{
    // Initializes magic bitboard arrays. Should be called before
    // using Attacks::bishop / Attacks::rook
	inline void init()
	{
		initmagicmoves();
	}

	// Return a bitboard of the knight attacks for a 
	// knight situated on the given square
	inline uint64_t knight(Square sq)
	{
		return BitMask::knight_attacks[sq];
	}

	// Return a bitboard of the king attacks for a 
	// king situated on the given square
	inline uint64_t king(Square sq)
	{
		return BitMask::king_attacks[sq];
	}

	// Return a bitboard of the bishop attacks for a 
	// bishop situated on the given square 
	// Diagonal and anti-diagonal attacks with respect to the current occupancy
	inline uint64_t bishop(Square sq, uint64_t occ)
	{
		return Bmagic(sq, occ);
	}

	// Return a bitboard of the rook attacks for a 
	// rook situated on the given square 
	// Vertical (file) and horizontal(rank) attacks with respect to the current occupancy
	inline uint64_t rook(Square sq, uint64_t occ)
	{
		return Rmagic(sq, occ);
	}

	// Return a bitboard of the queen attacks for a 
	// queen situated on the given square 

	// Diagonal and anti-diagonal attacks with respect to the current occupancy
	// Vertical (file) and horizontal(rank) attacks with respect to the current occupancy
	inline uint64_t queen(Square sq, uint64_t occ)
	{
		return Qmagic(sq, occ);
	}
	
	// Check whether the given square is directly attacked by any of the given color's 
	//pieces in the position. 
	inline bool square_attacked(Position const& position, Square sq, Color enemy, uint64_t occupancy)
	{
		assert(is_ok(sq));
		uint64_t pawns   = position.pieces.get_piece_bb<Pawn>(enemy);
		uint64_t knights = position.pieces.get_piece_bb<Knight>(enemy);
		uint64_t bishops = position.pieces.get_piece_bb<Bishop>(enemy);
		uint64_t rooks   = position.pieces.get_piece_bb<Rook>(enemy);
		uint64_t queens  = position.pieces.get_piece_bb<Queen>(enemy);
		uint64_t kings   = position.pieces.get_piece_bb<King>(enemy);

		bishops |= queens;
		rooks |= queens;


		return (BitMask::pawn_attacks[!enemy][sq] & pawns)
			|| (bishop(sq, occupancy) & bishops)
			|| (rook(sq, occupancy) & rooks)
			|| (knight(sq) & knights)
			|| (king(sq) & kings);
	}

    // Return a bitboard of all the attackers to the given square, both black and white pieces
    // are included. 
	inline uint64_t attackers_to_sq(Position const& position, Square sq)
	{
		uint64_t occ = position.total_occupancy();
		uint64_t pawn_mask = (BitMask::pawn_attacks[White][sq] & position.pieces.bitboards[Pawn] & position.pieces.colors[Black]);
		pawn_mask |= (BitMask::pawn_attacks[Black][sq] & position.pieces.bitboards[Pawn] & position.pieces.colors[White]);

		uint64_t bishops = position.pieces.bitboards[Bishop] | position.pieces.bitboards[Queen];
		uint64_t rooks = position.pieces.bitboards[Rook] | position.pieces.bitboards[Queen];


		return (pawn_mask)
			| (knight(sq) & position.pieces.bitboards[Knight])
			| (king(sq) & position.pieces.bitboards[King])
			| (bishop(sq, occ) & bishops)
			| (rook(sq, occ) & rooks);
	}

	inline bool square_attacked(Position const& position, Square sq, Color enemy)
	{
		return square_attacked(position, sq, enemy, position.total_occupancy());
	}

	// Check for the piece and generate (no pawns)
	inline uint64_t generate(PieceType piece, Square sq, uint64_t occ)
	{
		switch (piece)
		{
			case Knight:
				return knight(sq);

			case Bishop:
				return bishop(sq, occ);

			case Rook:
				return rook(sq, occ);

			case Queen:
				return queen(sq, occ);

			case King:
				return king(sq);

			default:
				throw std::invalid_argument("Invalid argument in generate");
				return 0;
		}
	}

}