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

#define DATASET     ("C:/tuning/lichess-quiet.txt")
#define NPOSITIONS  (2000005)
#define ADAM_BETA1  (0.9)
#define ADAM_BETA2  (0.999)
#define NTERMS      (689)
#define NPARTITIONS (32)
#define CHUNK       (NPOSITIONS / 8)
#define STACKSIZE  ((int)((double)NPOSITIONS * NTERMS / 64))

struct TTuple 
{
    int16_t index, coeff;
};

struct TPos 
{
    int16_t seval, phase, turn, ntuples;
    int eval;
    double result, pfactors[2];
    TTuple* tuples;
};

typedef double TCoeffs[NTERMS];
typedef double TVector[NTERMS][2];

void tune();