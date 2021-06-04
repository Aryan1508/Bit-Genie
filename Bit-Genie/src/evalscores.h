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

/********************** PAWN EVALUATION SCORES ********************/
namespace PawnEval
{
    constexpr int psqt[64]
    {
        S( 1, 0    ), S( 0, 0   ), S(   0, 0  ), S( 0, 1     ), S( -1, 0    ), S( 0, -1   ), S( 1, 0    ), S( 0, 0     ),
        S( -7, 12  ), S( -8, 15 ), S( -51, -16), S( -70, -28 ), S( -42, -12 ), S( -45, -8 ), S( -42, 11 ), S( -30, 38  ),
        S( 8, 52   ), S( 8, 32  ), S( -15, 10 ), S( -50, -20 ), S( -37, -51 ), S( 2, -39  ), S( 2, 13   ), S( -4, 28   ),
        S( 3, 60   ), S( 10, 22 ), S( -8, 25  ), S( -2, -55  ), S( 1, -27   ), S( -3, 4   ), S( 12, 30  ), S( -11, 73  ),
        S( -10, 78 ), S( -1, 54 ), S( -11, 7  ), S( -1, -51  ), S( -2, -4   ), S( -7, -9  ), S( -1, 63  ), S( -23, 77  ),
        S( -14, -8 ), S( -3, 35 ), S( -14, 2  ), S( -12, 10  ), S( -5, 4    ), S( -12, 39 ), S( 13, -42 ), S( -17, -21 ),
        S( -15, 27 ), S( 2, -24 ), S( -22, 54 ), S( -17, 17  ), S( -14, 52  ), S( 7, -45  ), S( 18, -59 ), S( -18, -52 ),
        S( 10, 5   ), S( 2, 0   ), S( 0, 0    ), S( 0, 0     ), S( 1, 1     ), S( -1, 0   ), S( 0, 0    ), S( 0, -1    ),
    };

    constexpr int passed[64] = {
        S(   0,   0 ), S(   0,   0  ), S(   0,   0 ), S(   0,   0 ), S(   0,   0 ), S(   0,   0 ), S(   0,   0 ), S(   0,   0 ),
        S( 224, 223 ), S( 221, 222  ), S( 225, 225 ), S( 231, 231 ), S( 214, 214 ), S( 215, 215 ), S( 213, 214 ), S( 230, 230 ),
        S( 136, 138 ), S( 129, 131  ), S( 122, 123 ), S( 130, 129 ), S( 119, 116 ), S( 100, 95  ), S( 115, 115 ), S( 124, 123 ),
        S(  55, 56  ), S( 43, 42    ), S( 42, 44   ), S( 31, 35   ), S( 24, 22   ), S( 32, 31   ), S( 37, 39   ), S( 47, 48   ),
        S(  22, 25  ), S( 14, 15    ), S( 11, 10   ), S( 3, 10    ), S( 6, 8     ), S( 5, 2     ), S( 15, 14   ), S( 23, 25   ),
        S(  -6, 7   ), S( -7, -6    ), S( -4, 1    ), S( -3, 3    ), S( -2, -3   ), S( -6, -4   ), S( -8, -9   ), S( 0, -2    ),
        S( -10, 0   ), S(   -10, -3 ), S( 3, -1    ), S( 1, 14    ), S( 9, 13    ), S( -9, -7   ), S( -11, -12 ), S( -6, -7   ),
        S(   0,   0 ), S(   0,   0  ), S(   0,   0 ), S(   0,   0 ), S(   0,   0 ), S(   0,   0 ), S(   0,   0 ), S(   0,   0 ),
    };

    constexpr int isolated = S(-10, -10);
    constexpr int stacked = S(-10, -11);
}

/********************** KNIGHT EVALUATION SCORES ********************/
namespace KnightEval
{
    constexpr int psqt[64]
    {
        S( -94, -82 ), S( -37, -36 ), S( -5, -7   ), S( -21, -19 ), S( -6, -2  ), S( -46, -45 ), S( -46, -47 ), S( -101, -101 ),
        S( -22, -10 ), S( -16, 7   ), S( 9, -1    ), S( 5, 10    ), S( 4, -4   ), S( -4, -5   ), S( -2, -4   ), S( -40, -25   ),
        S( -26, -20 ), S( 0, 0     ), S( 8, 0     ), S( 18, 17   ), S( 23, 22  ), S( 33, 23   ), S( 12, 11   ), S( -2, 0      ),
        S( -5, 1    ), S( 3, 4     ), S( 10, 10   ), S( 35, 32   ), S( 25, 23  ), S( 29, 21   ), S( 10, 9    ), S( 15, 11     ),
        S( -12, -6  ), S( -8, -6   ), S( 5, 5     ), S( 12, 18   ), S( 14, 12  ), S( 13, 12   ), S( 14, 14   ), S( 8, 3       ),
        S( -22, -17 ), S( -15, -14 ), S( -9, -12  ), S( 1, 1     ), S( 8, 6    ), S( -4, -4   ), S( 1, 0     ), S( -22, -21   ),
        S( -11, -12 ), S( -17, -9  ), S( -26, -14 ), S( -1, -13  ), S( -7, -7  ), S( -7, -21  ), S( -17, -10 ), S( -2, -16    ),
        S( -34, -34 ), S( -18, -14 ), S( -31, -23 ), S( -23, -15 ), S( -11, -8 ), S( -22, -20 ), S( -27, -28 ), S( -18, -11   ),
    };

    constexpr int mobility[9]
    {
        S(  -125, -115 ), S(  -26, -77 ),   S(  -13, -29 ),
        S(  4, 9       ), S(  21, 36   ),   S(  29, 55   ),
        S(  36, 59     ), S(  44, 48   ),   S(  49, 13   )
    };
}

/********************** BISHOP EVALUATION SCORES ********************/
namespace BishopEval
{
    constexpr int psqt[64]
    {
        S( -20, -10 ), S( -13, -12 ), S( -30, -17 ), S( -14, -2  ), S( -17, -14 ), S( -12, -7 ), S( -4, -1 ), S( -19, -14 ),
        S( -17, -15 ), S( -1, 1    ), S( -6, -2   ), S( -25, -21 ), S( -1, -3   ), S( 11, 4   ), S( 4, 0 ), S( -30, -19 ),
        S( -14, 3   ), S( 4, 3     ), S( 7, 2     ), S( 9, 2     ), S( 6, 0     ), S( 19, 17  ), S( 12, 5 ), S( 1, 5 ),
        S( -1, 2    ), S( -2, 0    ), S( 9, 8     ), S( 21, 22   ), S( 24, 22   ), S( 16, 15  ), S( 3, -7 ), S( 4, 1 ),
        S( -3, 1    ), S( 1, 1     ), S( 5, 5     ), S( 18, 16   ), S( 12, 10   ), S( 2, 9    ), S( -6, -8 ), S( 10, 5 ),
        S( 0, 3     ), S( 8, 8     ), S( 8, 9     ), S( 5, 7     ), S( 6, 5     ), S( 10, 8   ), S( 2, 2 ), S( 0, 0 ),
        S( 10, 5    ), S( 8, -3    ), S( 5, 5     ), S( -2, -1   ), S( 6, 3     ), S( 2, -2   ), S( 24, 20 ), S( 2, -1 ),
        S( -11, -11 ), S( 6, 3     ), S( 7, 7     ), S( -4, 0    ), S( -1, 0    ), S( -9, 4   ), S( -14, -7 ), S( -2, 4 ),
    };

    constexpr int mobility[14]
    {
        S( -80, -70 ),S(  -76, -67 ), S(  -18, -14 ), S(  -11, -11 ), S(  3, 4 ),   S(  12, 15 ),
        S(  23, 21 ), S(  28, 29 )  , S(  30, 31 ),   S(  41, 35 ),   S(  39, 40 ), S(  36, 30 ),
        S(  40, 34 ), S(  40, 39 )
    };
}

/********************** ROOK EVALUATION SCORES ********************/
namespace RookEval
{
    constexpr int psqt[64]
    {
        S( 15, 15   ), S( 12, 10  ), S( 11, 8   ), S( 13, 11   ), S( 14, 13  ), S( 8, 10   ), S( 15, 15   ), S( 11, 8    ),
        S( 17, 17   ), S( 16, 18  ), S( 22, 22  ), S( 23, 23   ), S( 11, 10  ), S( 18, 18  ), S( 11, 16   ), S( 18, 16   ),
        S( 5, 6     ), S( 10, 9   ), S( 6, 5    ), S( 5, 7     ), S( 1, 3    ), S( 7, 1    ), S( 10, 4    ), S( 0, -1    ),
        S( 3, 0     ), S( -3, -3  ), S( 4, 4    ), S( 0, 3     ), S( 1, -3   ), S( 15, 18  ), S( -3, 0    ), S( -1, -2   ),
        S( -11, -8  ), S( -7, 0   ), S( -2, 5   ), S( -9, -4   ), S( -7, -6  ), S( -8, -5  ), S( -4, -1   ), S( -17, -16 ),
        S( -18, -5  ), S( -12, 11 ), S( -16, -5 ), S( -19, -18 ), S( -14, -6 ), S( -11, -1 ), S( -11, -1  ), S( -29, -19 ),
        S( -18, -12 ), S( -8, -5  ), S( -12, -7 ), S( -8, -3   ), S( -12, -7 ), S( -3, -3  ), S( -12, -11 ), S( -39, 0   ),
        S( -5, -6   ), S( 1, 0    ), S( 3, 2    ), S( 6, 4     ), S( 5, 2    ), S( 14, 13  ), S( -26, -10 ), S( -22, -9  ),
    };

    constexpr int mobility[15]
    {
        S( -57, -69 ),   S( -28, -58 ), S( 0, -31 ), S( -13, -18 ), S( 5, 18 ), S( 5, 48 ), S( 15, 73 ),
        S( 30, 85 )  ,   S( 30, 82 ),   S( 32, 81 ), S( 36, 81 ),   S( 37, 82 ),S( 40, 84 ),S( 48, 66 ),
        S( 43, -10 )
    };

    constexpr int open_file = S(14, 8);
    constexpr int semi_open_file = S(20, 23);
}

/********************** QUEEN EVALUATION SCORES ********************/
namespace QueenEval
{
    constexpr int psqt[64]
    {
        S( -1, 3    ), S( 37, 38  ), S( 48, 49 ), S( 41, 42 ), S( 66, 66 ), S( 49, 48 ), S( 43, 41   ), S( 58, 57   ),
        S( -15, -15 ), S( -16, -9 ), S( 28, 20 ), S( 46, 43 ), S( 49, 47 ), S( 70, 70 ), S( 57, 57   ), S( 66, 58   ),
        S(  -7, 3   ), S( 6, 22   ), S( 16, 18 ), S( 38, 34 ), S( 59, 57 ), S( 73, 73 ), S( 72, 72   ), S( 72, 70   ),
        S(  1, 6    ), S( -3, 11  ), S( 6, 11  ), S( 7, 22  ), S( 25, 25 ), S( 37, 38 ), S( 50, 50   ), S( 26, 27   ),
        S(  0, 13   ), S( 1, 13   ), S( 13, 17 ), S( 5, 14  ), S( 16, 23 ), S( 25, 25 ), S( 35, 32   ), S( 27, 26   ),
        S(  -2, 2   ), S( 12, 13  ), S( 12, 21 ), S( 15, 20 ), S( 12, 18 ), S( 15, 21 ), S( 28, 29   ), S( 21, 23   ),
        S(  -9, -8  ), S( -1, 1   ), S( 24, 21 ), S( 18, 21 ), S( 27, 27 ), S( 14, 13 ), S( -1, 3    ), S( 5, 5     ),
        S(  12, 16  ), S( 3, 5    ), S( 16, 20 ), S( 30, 24 ), S( 13, 15 ), S( -9, -9 ), S( -17, -10 ), S( -35, -33 ),
    };

    constexpr int mobility[28]
    {
        S(-62,-48), S(-70,-36), S(-66,-49), S(-45,-50), S(-27,-46), S( -9,-45), S(  2,-37), S(  9,-23),
        S( 14, -7), S( 19,  9), S( 21, 24), S( 24, 34), S( 28, 39), S( 27, 49), S( 29, 55), S( 28, 63),
        S( 26, 69), S( 26, 69), S( 24, 73), S( 29, 72), S( 34, 74), S( 51, 63), S( 60, 69), S( 79, 66),
        S(106, 85), S(112, 84), S(104,111), S(108,131)
    };

    constexpr int open_file = S(2, 2);
    constexpr int semi_open_file = S(8, 12);
}

/********************** KINg EVALUATION SCORES ********************/
namespace KingEval
{
    constexpr int psqt[64]
    {
        S( -92, -75 ), S( -32, -27 ), S( -16, -2 ), S( -23, -24 ), S( -23, 0  ), S( 7, 12   ), S( 0, -1   ), S( -14, -8  ),
        S( -7, -12  ), S( 23, 30   ), S( 13, 17  ), S( 23, 9    ), S( 19, 23  ), S( 34, 29  ), S( 17, 18  ), S( 0, 6     ),
        S( 11, 11   ), S( 22, 29   ), S( 28, 9   ), S( 18, 23   ), S( 22, 20  ), S( 51, 49  ), S( 47, 43  ), S( 6, 14    ),
        S( -9, -3   ), S( 23, 19   ), S( 26, 25  ), S( 29, 33   ), S( 25, 28  ), S( 29, 36  ), S( 26, 33  ), S( -5, 7    ),
        S( -22, -11 ), S( 0, 3     ), S( 20, 23  ), S( 21, 28   ), S( 22, 23  ), S( 18, 23  ), S( 6, 4    ), S( -21, -15 ),
        S( -16, -15 ), S( 2, 1     ), S( 11, 14  ), S( 16, 23   ), S( 17, 25  ), S( 11, 17  ), S( 9, 12   ), S( -13, -9  ),
        S( -17, -19 ), S( 1, 5     ), S( 5, 12   ), S( -9, 93   ), S( -2, 65  ), S( 2, 9    ), S( 3, 10   ), S( -8, -12  ),
        S( -48, -58 ), S( 0, -56   ), S( 2, -47  ), S( -33, 42  ), S( -2, -75 ), S( -22, 12 ), S( 3, -118 ), S( -23, -83 ),
    };

    constexpr int attack_weight[5]
    {
      0, 2, 2, 3, 5
    };

    constexpr int safety_table[50]
    {
        S(  13,  13), S(   0,   0), S( -11,  -6), S(   0,   8),
        S(   2,   4), S(  -4,  -9), S(   1,  20), S( -17,  -7),
        S( -15,  -6), S( -40, -31), S( -58, -53), S( -64, -56),
        S( -64, -52), S( -64, -54), S( -92, -91), S( -94, -72),
        S(-115, -100), S(-105, -91), S(-115, -90), S(-139, -108),
        S(-180, -99), S(-163, -148), S(-163, -145), S(-243, -224),
        S(-151, -140), S(-204, -124), S(-268, -194), S(-129, -118),
        S(-223, -148), S(-141, -122), S( -82, -16), S(-174, -121),
        S(-169, -142), S(-339, -259), S(-300, -265), S( 144, -11),
        S(-329, -154), S(-198, -162), S(-258, -196), S(-210, -189),
        S(-189, -205), S(-211, -227), S(-234, -214), S(-233, -144),
        S(-322, -224), S( -98, -40), S(-336, -191), S(-126, -36),
        S(-134,   0), S( -61, -10),
    };
}
