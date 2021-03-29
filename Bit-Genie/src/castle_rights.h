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
#include "misc.h"
#include <string_view>

class CastleRights
{
public:
	CastleRights();

	void reset();

	bool parse_fen(std::string_view);

	uint64_t get_rooks(Color) const;

	void update(Move);

	uint64_t data() const { return rooks; }

	static bool castle_path_is_clear(Square rook, uint64_t);
	static uint64_t get_castle_path(Square);

	friend std::ostream& operator<<(std::ostream&, const CastleRights);
private:
	bool set(const char);

private:
	uint64_t rooks;
};

