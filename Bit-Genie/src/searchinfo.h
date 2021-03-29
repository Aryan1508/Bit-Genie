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
#include <utility>
#include <stdint.h>

struct SearchInfo
{
	uint64_t total_nodes = 0;
	uint64_t nodes = 0;
	uint64_t total_cutoffs = 0;
	int      ply = 0;
	int      depth = 0;
	int      seldepth = 0;

	void update_seldepth()
	{
		seldepth = std::max(seldepth, ply);
	}
};