
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

namespace PawnEval
{
    constexpr int stacked = S(-11, -12);

    constexpr int isolated = S(-16,  63);

    constexpr int passed_connected = S( 13,  41);

    constexpr int value = S(112, 215);

    constexpr int psqt[64]
    {
        S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), 
        S(  6,  12), S(  0,  15), S(-44, -16), S(-66, -28), S(-40, -12), S(-38,  -8), S(-35,  11), S(-21,  38), 
        S( 16,  57), S( 15,  32), S(  0,  10), S(-33, -20), S(-10, -51), S( 25, -39), S( 23,  13), S( 10,  33), 
        S(  0,  67), S(  1,  22), S( -5,  25), S(  5, -70), S(  5, -32), S(  1,   4), S( -1,  39), S( -9,  82), 
        S(-15,  94), S(-11,  54), S( -8,   3), S(  2, -59), S(  2, -10), S( -5,  -7), S(-12,  68), S(-24,  90), 
        S(-13,  -9), S(-14,  39), S(-11,   1), S( -9,   9), S(  1,   6), S( -4,  46), S(  1, -69), S(-17, -21), 
        S(-14,  36), S(-10, -33), S(-21,  70), S(-13,  19), S( -5,  63), S( 12, -50), S(  9, -90), S(-20, -52), 
        S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), 
    };

    constexpr int passed[64]
    {
        S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), 
        S(258, 254), S(247, 250), S(259, 257), S(268, 264), S(254, 250), S(250, 248), S(282, 284), S(290, 287), 
        S(157, 153), S(154, 153), S(143, 145), S(154, 162), S(128, 143), S(114, 121), S(147, 150), S(154, 154), 
        S( 76,  70), S( 65,  52), S( 55,  56), S( 35,  43), S( 28,  30), S( 42,  45), S( 68,  56), S( 69,  62), 
        S( 37,  36), S( 32,  27), S( 18,  17), S(  4,  15), S(  8,  11), S( 12,   8), S( 32,  22), S( 40,  36), 
        S(  3,  14), S(  5,   2), S(  0,   4), S(  3,   9), S( -8,  -2), S(-10,  -6), S(  0, -16), S(  8,  -2), 
        S(  3,   7), S(  7,   2), S( 14,   5), S(  8,  21), S( 13,  17), S( -8,  -4), S( -6, -15), S( 10,  -3), 
        S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), 
    };

    constexpr int passer_blocked[64]
    {
        S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), 
        S(121, 126), S(137, 136), S(142, 141), S(152, 154), S(125, 127), S( 96,  99), S( 72,  75), S( 71,  75), 
        S( 69,  72), S( 46,  52), S( 63,  71), S( 63,  72), S( 49,  61), S( 24,  29), S( 11,  20), S( 21,  47), 
        S( 34,  34), S( 22,  20), S( 15,  18), S( 12,  19), S(  9,  14), S( 11,  10), S( 15,   6), S(  2,   2), 
        S( 20,  12), S(  2,   0), S( -1,   1), S(-10,   3), S( -3,   1), S( -8,  -9), S(  1,  -2), S(  6,   4), 
        S(-20,  -5), S(-12,  -8), S( -8,  -3), S( -9,   5), S( -4,   0), S( -2,   0), S( -6, -12), S(  4,  -2), 
        S(-23, -19), S(-13, -12), S(  1,  -5), S( -6,  15), S(  6,  14), S( -4,  -5), S( -1,  -9), S( -9, -17), 
        S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), S(  0,   0), 
    };
}

namespace KnightEval
{
    constexpr int value = S(368, 250);

    constexpr int psqt[64]
    {
        S(-94, -82), S(-37, -36), S( -5,  -7), S(-22, -19), S( -6,  -2), S(-46, -45), S(-46, -47), S(-101, -101), 
        S(-26, -10), S(-16,   7), S(  9,  -1), S(  5,  10), S(  3,  -4), S( -4,  -5), S( -2,  -4), S(-36, -25), 
        S(-26, -20), S(  1,   0), S(  6,   0), S( 17,  17), S( 15,  22), S( 22,  23), S( 12,  11), S( -4,   0), 
        S( -5,   1), S(  5,   3), S( 10,  10), S( 34,  32), S( 25,  23), S( 24,  21), S( 10,   9), S( 12,  11), 
        S( -8,  -6), S( -6,  -6), S(  6,   5), S( 12,  18), S( 15,  12), S( 13,  13), S( 14,  14), S(  6,   3), 
        S(-21, -17), S(-17, -14), S( -9, -12), S(  1,   1), S(  8,   6), S( -3,  -5), S(  4,   0), S(-21, -21), 
        S(-11, -12), S(-18,  -9), S(-21, -14), S(  2, -14), S(  0,  -7), S( -5, -21), S(-13, -10), S( -2, -16), 
        S(-34, -34), S(-14, -15), S(-31, -23), S(-19, -15), S( -7,  -8), S(-17, -20), S(-13, -30), S(-18, -11), 
    };

    constexpr int mobility[9]
    {
        S(-125, -115), S(-23, -77), S(-14, -29), S(  6,  13), S( 20,  37), S( 27,  56), S( 36,  56), S( 44,  45), 
        S( 51, -23), 
    };
}

namespace BishopEval
{
    constexpr int value = S(401, 133);

    constexpr int psqt[64]
    {
        S(-20, -10), S(-13, -12), S(-30, -17), S(-14,  -2), S(-13, -14), S(-12,  -7), S( -4,  -1), S(-19, -14), 
        S(-15, -15), S( -2,   1), S( -6,  -2), S(-25, -21), S( -1,  -3), S(  8,   4), S(  3,   0), S(-35, -19), 
        S(-10,   3), S(  4,   3), S(  7,   2), S(  5,   2), S(  4,   0), S( 12,  17), S( 12,   5), S(  1,   5), 
        S( -1,   2), S( -1,   0), S(  8,   8), S( 20,  22), S( 18,  22), S( 11,  15), S(  1,  -7), S(  2,   1), 
        S( -3,   1), S(  2,   1), S(  4,   5), S( 16,  16), S( 12,  10), S(  2,   9), S( -4,  -8), S(  8,   5), 
        S(  0,   3), S(  6,   8), S(  7,   9), S(  6,   7), S(  9,   6), S( 11,   8), S(  8,   2), S(  0,   0), 
        S(  9,   5), S(  8,  -3), S(  3,   5), S( -1,  -1), S(  7,   4), S(  5,  -2), S( 24,  17), S( -3,  -1), 
        S(-11, -11), S(  3,   3), S(  4,   6), S(  0,   0), S(  1,   0), S( -1,   9), S(-14,  -7), S( -8,   4), 
    };

    constexpr int mobility[14]
    {
        S(-80, -70), S(-76, -67), S(-26, -13), S( -8,  -9), S(  7,   3), S( 16,  15), S( 23,  22), S( 31,  31), 
        S( 35,  32), S( 39,  33), S( 43,  39), S( 39,  29), S( 32,  34), S( 34,  39), 
    };
}

namespace RookEval
{
    constexpr int value = S(603, 679);

    constexpr int psqt[64]
    {
        S( 17,  15), S( 14,  10), S( 13,   8), S( 14,  11), S( 16,  13), S( 10,  10), S( 14,  15), S( 11,   8), 
        S( 17,  17), S( 16,  18), S( 20,  22), S( 19,  23), S( 11,  10), S( 17,  18), S( 11,  16), S( 16,  16), 
        S(  5,   6), S( 10,   9), S(  6,   5), S(  5,   7), S( -1,   3), S(  6,   1), S( 10,   4), S(  0,  -1), 
        S(  0,   0), S( -3,  -3), S(  5,   4), S(  0,   3), S( -1,  -3), S( 10,  18), S( -3,   0), S( -1,  -2), 
        S( -9,  -8), S( -4,   0), S( -2,   5), S( -8,  -4), S( -7,  -6), S( -8,  -5), S( -4,  -1), S(-17, -16), 
        S(-17,  -5), S(-12,  11), S(-16,  -5), S(-14, -18), S(-12,  -6), S(-10,  -1), S(-11,  -1), S(-28, -19), 
        S(-19, -12), S( -9,  -5), S(-12,  -7), S( -7,  -3), S( -9,  -7), S( -3,  -3), S(-12, -11), S(-39,   0), 
        S( -5,  -4), S( -3,   0), S(  0,   2), S(  4,   1), S(  5,  -2), S( 13,  12), S(-24,  -8), S(-19,  -7), 
    };

    constexpr int mobility[15]
    {
        S(-57, -69), S(-28, -58), S(-14, -34), S( -7, -18), S(  3,  20), S(  7,  49), S( 15,  73), S( 22,  80), 
        S( 28,  84), S( 32,  82), S( 39,  82), S( 41,  84), S( 44,  85), S( 49,  63), S( 46, -11), 
    };

    constexpr int open_file = S( 14,   1);

    constexpr int semi_open_file = S( 14,   1);
}

namespace QueenEval
{
    constexpr int value = S(1222, 1197);

    constexpr int psqt[64]
    {
        S( -3,   3), S( 36,  38), S( 41,  49), S( 41,  42), S( 64,  66), S( 49,  48), S( 43,  41), S( 58,  57), 
        S(-14, -15), S(-18,  -9), S( 27,  20), S( 46,  43), S( 37,  47), S( 53,  70), S( 55,  57), S( 63,  58), 
        S( -4,   3), S(  6,  22), S( 14,  18), S( 31,  34), S( 55,  57), S( 71,  73), S( 64,  72), S( 68,  70), 
        S(  1,   6), S( -3,  11), S(  5,  11), S(  2,  22), S( 24,  25), S( 33,  38), S( 44,  50), S( 24,  27), 
        S(  7,  13), S(  3,  13), S(  9,  17), S(  3,  14), S( 14,  23), S( 22,  25), S( 29,  32), S( 27,  26), 
        S(  3,   2), S( 15,  13), S( 12,  21), S( 15,  20), S( 15,  18), S( 20,  21), S( 28,  29), S( 21,  23), 
        S( -6,  -8), S(  6,   1), S( 25,  20), S( 25,  21), S( 34,  26), S( 23,  13), S( -1,   3), S(  8,   5), 
        S( 12,  16), S(  7,   5), S( 16,  20), S( 35,  24), S( 19,  15), S(  0,  -9), S(-12, -10), S(-35, -33), 
    };

    constexpr int mobility[28]
    {
        S(-62, -48), S(-70, -36), S(-66, -49), S(-45, -50), S(-27, -46), S(  4, -45), S(  2, -37), S(  9, -23), 
        S(  6,  -7), S( 13,  10), S( 16,  23), S( 16,  34), S( 21,  39), S( 20,  48), S( 25,  55), S( 27,  63), 
        S( 30,  69), S( 34,  69), S( 38,  73), S( 41,  72), S( 50,  75), S( 54,  63), S( 65,  69), S( 69,  66), 
        S( 78,  85), S( 86,  84), S(104, 111), S(108, 131), 
    };
}

namespace KingEval
{
    constexpr int psqt[64]
    {
        S(-92, -75), S(-32, -27), S(-16,  -2), S(-23, -24), S(-23,   0), S(  7,  12), S(  0,  -1), S(-14,  -8), 
        S( -7, -12), S( 23,  30), S( 13,  17), S( 23,   9), S( 19,  23), S( 34,  29), S( 17,  18), S(  0,   6), 
        S( 11,  11), S( 22,  29), S( 28,   9), S( 20,  23), S( 22,  20), S( 51,  49), S( 48,  43), S(  6,  14), 
        S( -9,  -3), S( 23,  19), S( 28,  25), S( 31,  33), S( 27,  28), S( 34,  36), S( 27,  33), S( -2,   7), 
        S(-22, -11), S(  0,   3), S( 22,  23), S( 23,  28), S( 26,  25), S( 20,  24), S(  7,   4), S(-18, -15), 
        S(-16, -15), S(  0,   1), S( 11,  14), S( 17,  23), S( 18,  26), S( 13,  18), S(  8,  11), S(-13,  -9), 
        S(-17, -19), S( -1,   5), S(  3,  12), S( -8,  93), S( -1,  69), S(  2,   9), S(  3,   9), S(-10, -16), 
        S(-48, -58), S( -1, -59), S( -1, -48), S(-34,  42), S( -9, -83), S(-21,  16), S(  3, -191), S(-28, -89), 
    };

    constexpr int attack_weight[5]
    {
        0, 2, 2, 3, 5
    };

    constexpr int safety_table[100]
    {
        S(  13,  13), S(   0,   0), S( -12,  -7), S(   0,   8), 
        S(   2,   4), S(  -4,  -9), S(   1,  20), S( -17,  -7),
        S( -15,  -6), S( -40, -31), S( -58, -53), S( -64, -58),
        S( -63, -50), S( -64, -54), S( -93, -92), S( -95, -75), 
        S(-114, -81), S(-104, -81), S(-112, -87), S(-140, -107),
        S(-183, -125), S(-158, -142), S(-163, -139), S(-242, -223),
        S(-150, -142), S(-205, -126), S(-263, -223), S(-131, -120), 
        S(-234, -151), S(-141, -127), S( -79, -16), S(-175, -122),
        S(-172, -145), S(-330, -250), S(-287, -268), S( 107,  -8),
        S(-323, -162), S(-197, -161), S(-256, -196), S(-210, -189), 
        S(-189, -205), S(-211, -227), S(-234, -214), S(-233, -144),
        S(-322, -224), S( -96, -40 ), S(-336, -191), S(-126, -36),
        S(-400, -400), S( -400, -10), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400),
    };
}

namespace MiscEval
{
    constexpr int control = S(  0,  13);
}
