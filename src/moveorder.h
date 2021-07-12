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

class MovePicker
{
public:
    enum class Stage
    {
        HashMove,
        GenNoisy,
        GiveGoodNoisy,
        Killer1,
        Killer2,
        GiveBadNoisy,
        GenQuiet,
        GiveQuiet
    };

    MovePicker(Search::Info&);

    bool next(Move &);
    bool qnext(Move &);

    Movelist movelist;
    Movelist::iterator current;
    Search::Info *search;

    Stage stage = Stage::HashMove;
    Move killer2   = NullMove;
    Move killer1   = NullMove;
    Move hash_move = NullMove;
    bool skip_quiets = false;
};