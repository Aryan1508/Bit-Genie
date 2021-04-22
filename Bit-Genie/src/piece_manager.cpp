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
#include "piece_manager.h"
#include "Square.h"
#include "stringparse.h"

static int to_digit(const char c) {
	return c - '0';
}

static bool contains_whitespaces(std::string_view str)
{
	return str.find(' ') != str.npos;
}

static bool is_valid_piece(const char piece)
{
	std::string valid_pieces = "pnbrqk";
	return valid_pieces.find(std::tolower(piece)) != valid_pieces.npos;
}

static bool is_valid_increment(const int increment)
{
	return increment > 0 && increment < 9;
}

PieceManager::PieceManager()
{
	reset();
}

void PieceManager::reset()
{
	colors.fill(0);
	bitboards.fill(0);
	squares.fill(Empty);
}

static inline Piece make_piece(char label)
{
	Color color = std::isupper(label) ? White : Black;
	label = std::tolower(label);
	PieceType type =
		label == 'p' ? Pawn :
		label == 'n' ? Knight :
		label == 'b' ? Bishop :
		label == 'r' ? Rook :
		label == 'q' ? Queen : King;
	return make_piece(type, color);
}

bool PieceManager::add_piece(const Square sq, const char label)
{
	if (!is_valid_piece(label))
	{
		return false;
	}
	add_piece(sq, make_piece(label));
	return true;
}

void PieceManager::add_piece(Square sq, Piece piece)
{
	squares[sq] = piece;
	set_bit(sq, bitboards[to_int(type_of(piece))]);
	set_bit(sq, colors[to_int(color_of(piece))]);
}

bool PieceManager::add_rank(Square& counter, std::string_view rank)
{
	for (auto const c : rank)
	{
		if (!is_ok(counter))
			return false;

		if (std::isalpha(c))
		{
			if (!add_piece(flip_square(counter), c))
			{
				return false;
			}
			counter++;
		}

		else if (std::isdigit(c))
		{
			const int increment = to_digit(c);
			if (!is_valid_increment(increment))
			{
				return false;
			}
			counter += increment;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool PieceManager::parse_fen_board(std::string_view fen)
{
	reset();

	if (contains_whitespaces(fen))
		return false;

	if (fen.size() == 0)
		return false;

	Square square_counter = Square::A1;

	for (auto const& rank : split_string(fen, '/'))
	{
		if (!add_rank(square_counter, rank))
		{
			return false;
		}
	}
	return true;
}

std::ostream& operator<<(std::ostream& o, PieceManager const& pm)
{
	for (Square sq = Square::A1; sq <= Square::H8; sq++)
	{
		if (sq % 8 == 0)
			o << '\n';

		o << pm.squares[to_int(flip_square(sq))] << ' ';
	}
	return o;
}