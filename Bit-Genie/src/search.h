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
#include "searchstats.h"
#include "searchlimits.h"
#include "shistory.h"
#include <atomic>

namespace Search 
{
    struct Info
    {
        Position*  position;
        Stats      stats;
        Limits     limits;
        Move       killers[64][2] = {NullMove};
        SHistory   history;
        SHistory   chistory;

        void update();
    };

    void init();
    uint64_t bestmove(Info, bool log);
}

extern std::atomic_bool SEARCH_ABORT;