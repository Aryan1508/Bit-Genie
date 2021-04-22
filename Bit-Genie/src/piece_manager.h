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
#include <array>
#include "bitboard.h"
#include "piece.h"

class PieceManager
{
	friend class Position;
public:
	PieceManager();
	void reset();
	bool parse_fen_board(std::string_view);


	uint64_t get_occupancy(Color color) const noexcept 
    {
        return colors[color];
    }

	Piece const& get_piece(Square sq) const noexcept
    {
        return squares[sq];
    }

	uint64_t get_piece_bb(Piece piece) const noexcept
    {
        return bitboards[type_of(piece)] & colors[color_of(piece)];
    }

	uint64_t get_piece_bb(PieceType pt, Color color) const noexcept
    {
        return bitboards[pt] & colors[color];
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