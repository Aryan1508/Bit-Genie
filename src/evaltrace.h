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

struct EvalTrace 
{
    int psqt[6][64];
    int mobility[5][28];
    int safety_table[100];
    int passer[8];
    int blocked_passer[8];
    int material[5];
    int pawn_shield[4];
    int stacked;
    int isolated;
    int supported_passer;
    int passer_tempo;
    int passer_edge_distance;
    int blocked_passer_edge_distance;
    int phalanx[8];
    int support;
    int open_file;
    int semi_open_file;
    int friendly_file;
    int friendly_rank;
    int control;
    int center_control;
    int connectivity[8];
    int eval;

    EvalTrace();

    void reset();
};