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
#include "Square.h"
#include "board.h"

namespace
{
	bool verify_sq_size(std::string_view sq)
	{
		return sq.size() == 2;
	}

	bool verify_sq_rank(const char rank)
	{
		std::string valid_ranks = "123456789";
		return valid_ranks.find(rank) != std::string::npos;
	}

	bool verify_sq_file(const char file)
	{
		std::string valid_files = "abcdefgh";
		return valid_files.find(file) != std::string::npos;
	}
}
bool is_valid_sq(std::string_view sq)
{
	return verify_sq_size(sq)
		&& verify_sq_file(sq[0])
		&& verify_sq_rank(sq[1]);
}

std::ostream& operator<<(std::ostream& o, const Square sq)
{
	if (sq == Square::bad_sq)
	{
		return o << '-';
	}
	assert(is_ok(sq));
	return o << file_of(sq) << rank_of(sq);
}