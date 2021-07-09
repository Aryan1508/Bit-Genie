
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

#define mg_score(s) ((int16_t)((uint16_t)((unsigned)((s)))))
#define eg_score(s) ((int16_t)((uint16_t)((unsigned)((s) + 0x8000) >> 16)))

namespace PawnEval
{
    constexpr int value = S(  99,  154);

    constexpr int stacked = S(  -3,  -16);

    constexpr int isolated = S( -18,  -16);

    constexpr int passed_connected = S(  18,    4);

    constexpr int passed_tempo = S(  45,   48);

    constexpr int support = S(  12,   11);

    constexpr int psqt[64]
    {
        S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
        S( -20,   74), S( -87,   60), S( -66,   23), S( -51,  -36), S( -36,  -19), S(-109,   30), S( -76,   33), S(-122,   73), 
        S( -12,   38), S(  -8,   14), S(  23,   11), S(  40,   16), S(  55,   24), S(  95,   -4), S(  61,   21), S(  28,   35), 
        S( -35,   26), S( -14,    3), S(   1,   -3), S(   1,  -15), S(  30,  -21), S(  20,  -13), S(   8,   -6), S(  -5,   -1), 
        S( -43,    2), S( -25,  -10), S(  -4,  -15), S(  11,  -12), S(  17,  -15), S(  10,  -18), S(  -5,  -17), S( -18,  -20), 
        S( -50,   -3), S( -32,  -14), S( -14,  -11), S(  -7,  -12), S(   9,   -7), S(  -5,   -8), S(  13,  -30), S( -15,  -23), 
        S( -44,    1), S( -22,  -12), S( -16,   -5), S( -23,   -1), S(  -6,    8), S(  -2,    3), S(  19,  -22), S( -34,  -18), 
        S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
    };

    constexpr int passed[64]
    {
        S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
        S( 303,  243), S( 122,  337), S( 166,  355), S( 202,  329), S( 208,  321), S( 103,  310), S( 189,  358), S( 220,  302), 
        S(  42,  251), S(  45,  267), S(   4,  206), S(   9,  121), S( -11,  120), S( -55,  236), S(  23,  246), S( -40,  261), 
        S(   8,  143), S( -11,  142), S( -20,   85), S(  -9,   62), S( -25,   65), S( -20,   94), S( -43,  156), S( -26,  153), 
        S( -19,   79), S( -25,   58), S( -42,   35), S( -41,   11), S( -53,   27), S( -60,   55), S( -57,   88), S( -14,   80), 
        S( -17,   10), S( -61,   13), S( -41,   -6), S( -49,  -10), S( -50,  -13), S( -56,   -5), S( -63,   32), S( -14,   16), 
        S( -18,    3), S( -29,   -2), S( -35,  -13), S( -28,  -23), S( -30,  -32), S( -22,  -22), S( -11,   -8), S(  -1,    1), 
        S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
    };

    constexpr int passer_blocked[64]
    {
        S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
        S(  63,  191), S( 202,   88), S( 230,   63), S( 226,   93), S( 172,   67), S( 194,   77), S( 127,   23), S(  64,   91), 
        S(  53,  102), S( 120,   65), S(  96,   13), S(  31,    5), S(  18,  -12), S(  54,   20), S(  33,   12), S( -49,   27), 
        S(  39,   69), S(  51,   59), S(  27,   38), S(  22,   32), S(  -5,   32), S(  -3,   34), S(  34,   35), S(  -4,   45), 
        S(  39,   36), S(  22,   32), S( -13,   21), S( -17,   22), S( -22,   30), S( -29,   33), S(  -2,   43), S( -20,   41), 
        S(  20,   -9), S(  19,   29), S( -26,   13), S( -33,   12), S( -40,   23), S( -40,   33), S( -33,   52), S( -26,   30), 
        S(  11,  -12), S(   2,   14), S( -26,   23), S( -27,   26), S(  -1,   -1), S( -14,    0), S(   6,   -8), S(   7,    8), 
        S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
    };
}

namespace KnightEval
{
    constexpr int value = S( 400,  500);

    constexpr int mobility[9]
    {
        S(-125, -115), S( -49,  -61), S( -11,   16), S(   3,   44), S(  20,   44), S(  24,   46), S(  34,   48), S(  44,   41), 
        S(  54,   34), 
    };

    constexpr int psqt[64]
    {
        S(-176,  -33), S(-127,  -11), S( -75,   15), S(-118,   26), S( -39,   14), S(-104,  -70), S(-101,  -43), S(-123,  -55), 
        S( -49,   -1), S( -38,   24), S(  -3,   18), S(  49,   -9), S( -28,   -1), S(  79,  -25), S( -14,    5), S( -25,  -20), 
        S( -33,    1), S(  23,    8), S(  22,   27), S(  37,   25), S(  83,   -4), S(  86,    3), S(  44,    4), S(   0,   -4), 
        S(  -1,   27), S(   3,   27), S(  33,   30), S(  60,   29), S(  32,   26), S(  52,   35), S(  12,   25), S(  35,   -5), 
        S( -13,   12), S(  -4,   21), S(  10,   25), S(  16,   33), S(  22,   33), S(  18,   15), S(  10,   19), S(   0,   10), 
        S( -42,   -7), S( -22,    8), S(  -6,   -7), S(  -3,   26), S(  15,   14), S(  -2,   -6), S(   8,   -3), S( -22,   -7), 
        S( -35,   14), S( -31,    1), S( -24,    0), S(   2,   -5), S(  -4,  -10), S(  -6,  -14), S(  -6,    5), S(  -9,   28), 
        S( -70,   38), S( -30,   -9), S( -58,    9), S( -36,    5), S( -25,   -2), S( -21,   -8), S( -25,   -3), S( -41,   15), 
    };
}

namespace BishopEval
{
    constexpr int value = S( 435,  472);

    constexpr int mobility[14]
    {
        S( -80,  -70), S( -76,  -67), S( -34,   11), S( -15,   50), S(   2,   47), S(   9,   56), S(  26,   74), S(  37,   78), 
        S(  43,   74), S(  46,   79), S(  51,   77), S(  49,   63), S(  53,   36), S(  61,   39), 
    };

    constexpr int psqt[64]
    {
        S( -47,   43), S( -89,   65), S( -47,   40), S(-119,   54), S(-113,   56), S( -75,   32), S( -28,   19), S( -59,   22), 
        S( -24,   17), S( -16,   19), S( -19,   27), S( -38,   32), S( -11,    9), S( -30,   18), S( -26,   19), S( -37,   12), 
        S(   7,   38), S(  25,   29), S(  16,   21), S(  33,    3), S(  22,    1), S(  65,   22), S(  29,   32), S(  34,   30), 
        S( -16,   43), S(   3,   39), S(  18,   18), S(  32,   33), S(  38,   25), S(  10,   24), S(   8,   19), S( -25,   46), 
        S(  -2,   39), S(  -7,   25), S(   1,   27), S(  22,   33), S(  22,   21), S(   5,   14), S(  -4,   15), S(  10,   12), 
        S(   9,    5), S(  13,   31), S(   8,   26), S(  11,   19), S(  10,   28), S(  15,   16), S(  19,    9), S(  18,   19), 
        S(  19,   23), S(  17,    9), S(  20,   -2), S(   0,   11), S(  11,   15), S(  19,   -1), S(  44,   10), S(  24,  -16), 
        S(  20,   22), S(  23,   -1), S(   8,   11), S(  -6,   26), S(   4,   32), S(  -4,   34), S(   7,    4), S(  43,  -16), 
    };
}

namespace RookEval
{
    constexpr int value = S( 543,  882);

    constexpr int open_file = S(  30,   12);

    constexpr int semi_open_file = S(  26,    0);

    constexpr int mobility[15]
    {
        S( -57,  -69), S( -28,  -58), S( -16,   51), S(   0,   72), S(  10,   84), S(  14,   75), S(  18,   74), S(  22,   83), 
        S(  27,   68), S(  32,   66), S(  35,   69), S(  39,   63), S(  39,   65), S(  44,   54), S(  47,   32), 
    };

    constexpr int psqt[64]
    {
        S(   4,   50), S( -50,   74), S(   9,   65), S( -21,   76), S( -18,   62), S(  12,   65), S(  -2,   56), S(  43,   40), 
        S( -19,   59), S( -35,   85), S( -15,   90), S(   8,   69), S( -16,   74), S(  11,   63), S(  10,   63), S(  63,   29), 
        S( -38,   61), S(  -6,   58), S( -22,   66), S( -25,   44), S(  18,   22), S(  27,   29), S(  98,    7), S(  37,    7), 
        S( -35,   58), S( -37,   63), S( -23,   55), S( -17,   40), S( -15,   24), S(  -9,   26), S(  12,   25), S(  -7,   28), 
        S( -36,   42), S( -40,   36), S( -37,   36), S( -26,   34), S( -21,   29), S( -35,   34), S(   9,   -3), S(  -9,   11), 
        S( -38,   28), S( -35,   21), S( -35,   30), S( -26,   24), S( -10,    8), S(  -9,    5), S(  17,  -16), S(   4,  -20), 
        S( -27,    7), S( -33,   17), S( -22,   15), S( -18,   12), S(  -7,    8), S(   1,   -3), S(  11,  -14), S(  -7,  -12), 
        S( -13,   19), S( -16,   18), S( -17,   27), S(  -8,   13), S(   1,    8), S(   3,   12), S(  22,   -5), S(  -3,   -8), 
    };
}

namespace QueenEval
{
    constexpr int value = S(1236, 1659);

    constexpr int mobility[28]
    {
        S( -62,  -48), S( -70,  -36), S( -66,  -49), S( -45,  -50), S( -27,  -46), S(   4,  -45), S(  -7,  -37), S( -11,  -23), 
        S(   0,   -6), S(   5,   17), S(  12,   28), S(  17,   35), S(  23,   41), S(  28,   47), S(  31,   52), S(  35,   62), 
        S(  36,   69), S(  41,   72), S(  41,   73), S(  45,   73), S(  49,   76), S(  53,   63), S(  57,   69), S(  65,   66), 
        S(  60,   85), S(  82,   84), S( 104,  111), S( 108,  131), 
    };

    constexpr int psqt[64]
    {
        S(   6,   33), S( -39,   72), S( -18,   80), S(  46,   19), S( -35,  119), S(  30,   84), S(  14,   86), S( -24,   82), 
        S(   1,   62), S( -28,   97), S( -34,  128), S( -38,  132), S( -50,  160), S( -26,  161), S( -42,  153), S(  25,  132), 
        S(  10,   53), S(  10,   37), S(  -7,   88), S(  -3,   74), S(  -8,  109), S(  15,  126), S(  15,  116), S(  41,   49), 
        S(  -8,   95), S(  16,   45), S( -16,   84), S(  -1,   72), S( -19,  101), S(  -6,   74), S( -12,  125), S(  -3,   92), 
        S(  13,   65), S(   0,   67), S(  11,   32), S(  10,   54), S(  15,   36), S(   2,   47), S(  17,   66), S(  25,   53), 
        S(   9,   48), S(  27,   16), S(  16,   43), S(  19,   23), S(  26,   22), S(  19,   49), S(  43,   23), S(  37,    7), 
        S(  21,   50), S(  30,    9), S(  35,    8), S(  39,    6), S(  40,    2), S(  45,  -20), S(  59,  -58), S(  81,  -80), 
        S(  33,    1), S(  29,    4), S(  30,   15), S(  35,   24), S(  37,   -2), S(  27,   -7), S(  51,   -7), S(  40,   17), 
    };
}

namespace KingEval
{
    constexpr int pawn_shield[4]
    {
        S( -48,   18), S( -11,    3), S(  21,   -6), S(  44,  -35), 
    };

    constexpr int psqt[64]
    {
        S(-127, -203), S(  32,  -89), S(  -5,  -36), S( -30,    5), S(  -1,   24), S(  -2,    5), S( -13,  -34), S( -32, -121), 
        S(   7,   12), S(  45,   45), S(  34,   77), S(  47,   57), S(   0,   93), S(  24,   68), S(  19,   88), S( -61,   29), 
        S( -81,   36), S(  30,   72), S(  29,   83), S( -68,  108), S( -40,   99), S(  22,   89), S(  29,   73), S( -65,   44), 
        S( -91,   24), S( -26,   54), S( -82,   87), S(-128,  112), S( -66,   98), S( -48,   90), S( -89,   73), S(-125,   40), 
        S( -85,   19), S( -69,   35), S( -13,   55), S( -66,   71), S( -49,   63), S( -69,   49), S( -71,   30), S(-160,   28), 
        S( -15,  -12), S(  37,   -8), S(  12,   15), S(  -2,   29), S(  -5,   29), S(  12,    4), S(  45,  -22), S( -11,   -9), 
        S(  81,  -36), S(  63,  -31), S(  48,  -15), S(  11,   -5), S(  12,   -3), S(  32,  -15), S(  81,  -47), S(  61,  -44), 
        S(  52,  -67), S(  87,  -58), S(  69,  -38), S( -28,  -14), S(  46,  -55), S( -11,  -14), S(  70,  -59), S(  72,  -91), 
    };

    constexpr int safety_table[100]
    {
        S(  13,   13), S(   9,    5), S(   3,    3), S(  -2,    4), S( -13,   12), S( -20,   14), S( -23,   11), S( -16,    1), 
        S(  -9,   -5), S( -19,   -9), S( -62,   81), S( -62,   65), S( -48,   97), S( -83,   94), S( -84,   26), S(-122,   29), 
        S(-133,   31), S(-131,   29), S(-140,   16), S(-161,  -37), S(-214,  -86), S(-195,   12), S(-216,  -63), S(-209, -127), 
        S(-163, -192), S(-265, -266), S(-202, -260), S(-225, -171), S(-264,  -93), S(-220, -115), S(-255, -110), S(-266, -157), 
        S(-264, -187), S(-333, -247), S(-350, -276), S( -84, -100), S(-362, -186), S(-293, -198), S(-311, -210), S(-249, -211), 
        S(-225, -221), S(-223, -232), S(-248, -219), S(-243, -148), S(-323, -224), S( -98,  -41), S(-337, -192), S(-124,  -35), 
        S(-400, -400), S(-400,  -10), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
        S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
    };

    constexpr int attack_weight[5]
    {
        0, 2, 2, 3, 5
    };
}

namespace MiscEval
{
    constexpr int control = S(  -2,    5);
}
