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
#include "search.h"
#include "movelist.h"

enum MovePickStage : uint8_t {
    STAGE_HASH_MOVE,
    STAGE_GEN_NOISY,
    STAGE_GOOD_NOISY,
    STAGE_KILLER_1,
    STAGE_KILLER_2,
    STAGE_BAD_NOISY,
    STAGE_GEN_QUIET,
    STAGE_QUIET
};

class MovePicker {
public:
    MovePicker(SearchInfo &);

    bool next(Move &);
    bool qnext(Move &);

    Movelist movelist;
    MovePickStage stage = STAGE_HASH_MOVE;
    bool skip_quiets    = false;
    Move hash_move      = MOVE_NULL;
    Move killer1        = MOVE_NULL;
    Move killer2        = MOVE_NULL;
private:
    SearchInfo *search;
    Movelist::iterator current;
};