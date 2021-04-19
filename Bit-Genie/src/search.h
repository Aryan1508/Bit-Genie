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
#include "searchinfo.h"
#include "searchlimits.h"
#include "killer.h"
#include "shistory.h"
#include <atomic>

struct Search
{
	SearchInfo   info;
	SearchLimits limits;
	Killers  killers;
	SHistory history;
};

void init_lmr_array();
void search_position(Position&, Search, TTable& tt);
uint64_t bench_search_position(Position&, TTable&);

extern std::atomic_bool SEARCH_ABORT;