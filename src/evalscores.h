
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

constexpr int PAWN_VALUE = S(  98,  154);

constexpr int PAWN_STACKED = S(  -4,  -16);

constexpr int PAWN_ISOLATED = S( -17,  -17);

constexpr int SUPPORTED_PASSER = S(  17,    5);

constexpr int PASSER_TEMPO = S(  42,   51);

constexpr int PAWN_SUPPORT = S(  12,   12);

constexpr int PAWN_PSQT[64]
{
    S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
    S( -36,   84), S( -93,   66), S( -77,   29), S( -61,  -31), S( -48,  -13), S(-123,   37), S( -87,   39), S(-137,   80), 
    S( -13,   36), S(  -5,    8), S(  26,    4), S(  40,   16), S(  54,   23), S(  94,   -7), S(  59,   17), S(  23,   36), 
    S( -36,   25), S( -11,   -1), S(   1,   -6), S(   3,  -18), S(  31,  -23), S(  18,  -14), S(   9,   -8), S(  -7,    1), 
    S( -44,    2), S( -23,  -14), S(  -3,  -18), S(  11,  -15), S(  17,  -18), S(  10,  -20), S(  -4,  -19), S( -20,  -18), 
    S( -49,   -4), S( -30,  -18), S( -13,  -14), S(  -7,  -14), S(   9,   -9), S(  -6,   -9), S(  13,  -32), S( -16,  -22), 
    S( -42,   -1), S( -19,  -16), S( -16,   -7), S( -22,   -3), S(  -6,    6), S(   0,    0), S(  19,  -24), S( -34,  -18), 
    S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
};

constexpr int PASSER[64]
{
    S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
    S( 285,  252), S( 114,  347), S( 151,  366), S( 191,  339), S( 196,  332), S(  88,  323), S( 171,  372), S( 198,  314), 
    S(  26,  265), S(  30,  282), S(  -5,  218), S(   2,  125), S( -18,  126), S( -68,  250), S( -13,  266), S( -42,  271), 
    S(   1,  150), S( -18,  149), S( -22,   88), S( -12,   65), S( -28,   67), S( -19,   96), S( -48,  161), S( -32,  158), 
    S( -21,   83), S( -29,   61), S( -44,   36), S( -43,   13), S( -53,   28), S( -60,   57), S( -59,   91), S( -15,   81), 
    S( -19,   12), S( -61,   15), S( -41,   -5), S( -50,  -10), S( -53,  -12), S( -61,   -4), S( -66,   33), S( -15,   16), 
    S( -20,    6), S( -31,    0), S( -36,  -12), S( -29,  -21), S( -27,  -32), S( -28,  -22), S( -12,   -9), S(   0,    1), 
    S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
};

constexpr int BLOCKED_PASSER[64]
{
    S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
    S(  64,  191), S( 204,   83), S( 233,   57), S( 229,   86), S( 173,   61), S( 198,   70), S( 134,   14), S(  73,   86), 
    S(  47,  110), S( 109,   76), S(  86,   22), S(  29,    5), S(  13,   -9), S(  49,   27), S(  29,   19), S( -53,   32), 
    S(  37,   75), S(  48,   63), S(  27,   40), S(  21,   33), S(  -6,   33), S(  -2,   35), S(  24,   40), S(  -8,   48), 
    S(  38,   39), S(  22,   34), S( -12,   22), S( -16,   23), S( -22,   32), S( -31,   34), S(  -2,   46), S( -21,   43), 
    S(  20,   -8), S(  19,   30), S( -25,   12), S( -34,   13), S( -41,   24), S( -38,   33), S( -38,   54), S( -28,   32), 
    S(  11,  -11), S(   2,   15), S( -25,   24), S( -25,   25), S(  -2,    1), S( -13,    0), S(   7,   -6), S(   4,   11), 
    S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), S(   0,    0), 
};

constexpr int KNIGHT_VALUE = S( 381,  510);

constexpr int KNIGHT_MOBILITY[9]
{
    S(-140, -176), S( -34,  -63), S(  -6,   -3), S(   3,   31), S(  16,   47), S(  19,   58), S(  29,   61), S(  40,   61), 
    S(  53,   59), 
};

constexpr int KNIGHT_PSQT[64]
{
    S(-181,  -26), S(-165,   19), S( -83,   26), S(-133,   39), S( -39,   22), S( -97,  -66), S(-148,  -16), S(-131,  -49), 
    S( -61,   24), S( -44,   33), S(  -1,   11), S(  56,  -19), S( -16,  -13), S(  78,  -34), S( -22,   15), S( -36,    2), 
    S( -38,    9), S(  24,    0), S(  28,    3), S(  44,    0), S(  88,  -29), S(  92,  -20), S(  45,   -7), S(  -1,    4), 
    S(  -8,   40), S(   6,   17), S(  41,    7), S(  71,    6), S(  46,    1), S(  63,    9), S(  15,   13), S(  30,    6), 
    S( -22,   29), S(  -2,   13), S(  19,    1), S(  26,   13), S(  35,   10), S(  32,   -9), S(  20,    9), S(  -6,   28), 
    S( -50,    9), S( -21,    1), S(   2,  -31), S(   3,    0), S(  24,  -10), S(   6,  -31), S(  11,   -9), S( -29,    8), 
    S( -49,   37), S( -39,   12), S( -24,   -9), S(   3,  -12), S(  -2,  -18), S(  -1,  -24), S( -13,   16), S( -22,   52), 
    S( -91,   65), S( -41,   12), S( -62,   16), S( -37,   10), S( -30,    6), S( -25,    0), S( -35,   16), S( -66,   43), 
};

constexpr int BISHOP_VALUE = S( 419,  484);

constexpr int FRIENDLY_DIAGONAL = S(   8,   -8);

constexpr int BISHOP_MOBILITY[14]
{
    S( -80,  -70), S( -90,   20), S( -60,   30), S( -42,   64), S( -14,   60), S(  -3,   68), S(  19,   75), S(  36,   73), 
    S(  50,   72), S(  58,   65), S(  68,   64), S(  76,   45), S(  79,   36), S( 120,   15), 
};

constexpr int BISHOP_PSQT[64]
{
    S( -40,   35), S( -85,   64), S( -41,   36), S(-119,   53), S(-107,   52), S( -66,   28), S( -32,   18), S( -50,   18), 
    S( -21,   14), S( -24,   21), S( -26,   28), S( -42,   32), S( -13,    8), S( -31,   14), S( -26,   14), S( -21,    0), 
    S(   5,   38), S(  18,   29), S(   7,   21), S(  23,    2), S(  11,   -1), S(  56,   21), S(  22,   31), S(  34,   26), 
    S( -15,   41), S(  -4,   41), S(   8,   17), S(  21,   29), S(  31,   19), S(   1,   22), S(   9,   16), S( -25,   44), 
    S(  -3,   37), S( -14,   26), S(  -9,   25), S(  15,   29), S(  13,   17), S(  -2,   12), S( -11,   17), S(  17,    7), 
    S(  10,    2), S(   8,   30), S(   4,   24), S(   2,   19), S(   4,   26), S(   8,   17), S(  17,    6), S(  21,   15), 
    S(  24,   16), S(  17,    3), S(  15,   -3), S(  -3,   10), S(   4,   17), S(  18,   -3), S(  44,    4), S(  27,  -20), 
    S(  27,   16), S(  28,   -5), S(  13,    8), S(  -3,   23), S(   8,   27), S(   2,   30), S(  13,    1), S(  48,  -24), 
};

constexpr int ROOK_VALUE = S( 503,  912);

constexpr int OPEN_FILE = S(  27,   11);

constexpr int SEMI_OPEN_FILE = S(  23,   -2);

constexpr int FRIENDLY_FILE = S(  11,  -10);

constexpr int FRIENDLY_RANK = S(   3,   -8);

constexpr int ROOK_MOBILITY[15]
{
    S( -57,  -69), S( -28,  -58), S( -36,   89), S( -18,   99), S(  -7,  101), S(   5,   96), S(  14,   88), S(  17,   85), 
    S(  28,   69), S(  40,   63), S(  46,   64), S(  54,   55), S(  64,   49), S(  63,   45), S(  77,   21), 
};

constexpr int ROOK_PSQT[64]
{
    S(   1,   45), S( -51,   68), S(   1,   60), S( -30,   71), S( -25,   57), S(   5,   61), S(  -2,   49), S(  36,   36), 
    S( -23,   56), S( -40,   82), S( -22,   88), S(   0,   67), S( -21,   71), S(   0,   60), S(   5,   59), S(  51,   26), 
    S( -37,   56), S(  -6,   53), S( -21,   61), S( -22,   37), S(  22,   16), S(  25,   24), S(  92,    4), S(  37,    1), 
    S( -34,   53), S( -36,   58), S( -24,   52), S( -20,   36), S( -17,   21), S( -10,   21), S(  11,   21), S(  -7,   23), 
    S( -35,   38), S( -40,   32), S( -38,   32), S( -30,   32), S( -28,   28), S( -37,   29), S(  10,   -8), S(  -8,    5), 
    S( -37,   23), S( -36,   17), S( -38,   27), S( -31,   21), S( -15,    5), S( -12,    0), S(  18,  -21), S(   5,  -26), 
    S( -25,    3), S( -36,   15), S( -26,   13), S( -24,   11), S( -13,    7), S(  -3,   -7), S(   7,  -17), S( -12,  -15), 
    S( -16,   16), S( -21,   12), S( -24,   21), S( -16,    8), S(  -8,    4), S(  -3,    9), S(  15,   -9), S(  -8,   -9), 
};

constexpr int QUEEN_VALUE = S(1026, 1856);

constexpr int QUEEN_MOBILITY[28]
{
    S( -62,  -48), S( -70,  -36), S( -66,  -49), S( -54,  -58), S( -88,  -73), S( -78,    7), S( -48,    7), S( -54,  101), 
    S( -37,   97), S( -32,  114), S( -23,  114), S( -13,  104), S(  -4,  111), S(   6,  100), S(  15,   93), S(  20,   96), 
    S(  26,   95), S(  35,   92), S(  39,   86), S(  49,   74), S(  57,   68), S(  63,   65), S(  70,   50), S(  72,   50), 
    S(  88,   17), S(  91,   12), S(  94,   46), S(  88,   74), 
};

constexpr int QUEEN_PSQT[64]
{
    S(   1,   21), S( -42,   75), S( -20,   88), S(  55,   25), S( -45,  138), S(  17,  107), S(  -4,  106), S( -30,  102), 
    S(  -8,   43), S( -38,   95), S( -41,  134), S( -42,  140), S( -36,  159), S( -17,  163), S( -39,  158), S(  28,  120), 
    S(   4,   41), S(  11,   36), S( -11,  103), S(   6,   90), S(   0,  135), S(  40,  152), S(  40,  136), S(  67,   54), 
    S( -15,   87), S(   3,   63), S( -23,  105), S(  -8,  108), S( -20,  136), S(   2,   96), S(   4,  121), S(  15,   86), 
    S(  -2,   64), S( -12,   79), S(  -2,   55), S(  -4,   89), S(   2,   71), S(  -1,   68), S(   9,   75), S(  28,   45), 
    S(  -7,   42), S(  11,   21), S(  -1,   64), S(   3,   46), S(  10,   47), S(   6,   64), S(  31,   28), S(  28,   -4), 
    S(   3,   43), S(  14,   11), S(  18,   14), S(  21,   20), S(  19,   19), S(  28,   -4), S(  43,  -56), S(  66,  -89), 
    S(  15,    0), S(  10,    5), S(  10,   25), S(  19,   30), S(  20,    4), S(   6,   -2), S(  38,  -11), S(  20,   16), 
};

constexpr int PAWN_SHIELD[4]
{
    S( -47,   17), S( -11,    3), S(  21,   -6), S(  45,  -34), 
};

constexpr int KING_PSQT[64]
{
    S(-144, -252), S(  99, -106), S(   2,  -38), S( -42,    8), S(  15,   23), S( -12,    8), S( -26,  -36), S( -30, -151), 
    S(  15,   19), S(  56,   45), S(  30,   79), S(  47,   58), S( -26,  101), S(  14,   72), S(   7,   91), S(-102,   42), 
    S(-128,   50), S(  31,   74), S(  28,   85), S( -91,  112), S( -51,  102), S(  19,   92), S(  26,   75), S( -89,   55), 
    S(-111,   36), S( -26,   55), S( -87,   89), S(-149,  117), S( -69,   99), S( -47,   91), S( -94,   74), S(-129,   48), 
    S( -94,   28), S( -64,   34), S(  -6,   54), S( -60,   71), S( -46,   64), S( -69,   49), S( -65,   28), S(-155,   32), 
    S(  -8,   -8), S(  49,  -12), S(  18,   14), S(   2,   29), S(  -7,   30), S(   9,    5), S(  46,  -23), S( -10,   -4), 
    S(  83,  -31), S(  69,  -34), S(  52,  -17), S(  17,   -7), S(  14,   -4), S(  36,  -17), S(  82,  -48), S(  62,  -40), 
    S(  50,  -58), S(  92,  -55), S(  78,  -37), S( -16,  -15), S(  54,  -54), S(  -4,  -11), S(  77,  -58), S(  73,  -83), 
};

constexpr int KING_SAFETY_TABLE[100]
{
    S(  13,   13), S(   0,    0), S(  24,  -30), S( -38,   15), S( -28,   17), S( -37,   10), S( -41,   14), S( -17,  -17), 
    S( -54,   41), S( -28,  -50), S( -87,    8), S( -70,  -16), S( -43,  -45), S(-105,   40), S( -73,  -63), S(-126,  -83), 
    S(-126,   31), S(-105,  -29), S(-128,   41), S(-137,  -38), S(-206,  -90), S(-228,   26), S(-221,    7), S(-173,   -3), 
    S(-196,  -79), S(-201, -188), S(-242, -224), S(-225, -165), S(-291,  -60), S(-240, -113), S(-254,  -80), S(-287, -183), 
    S(-295, -210), S(-363, -277), S(-415, -274), S(-102,  -83), S(-372, -229), S(-322, -208), S(-377, -228), S(-245, -200), 
    S(-160, -187), S(-229, -235), S(-234, -214), S(-235, -145), S(-323, -224), S( -96,  -40), S(-353, -198), S(-126,  -36), 
    S(-400, -400), S(-400,  -10), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
    S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
    S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
    S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
    S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
    S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
    S(-400, -400), S(-400, -400), S(-400, -400), S(-400, -400), 
};

constexpr int KING_ATTACK_WEIGHT[5]
{
    0, 2, 2, 3, 5
};

constexpr int CONTROL = S(  -3,    7);
