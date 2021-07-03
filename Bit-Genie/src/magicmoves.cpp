/**
 *Copyright (C) 2007 Pradyumna Kannan.
 *
 *This code is provided 'as-is', without any expressed or implied warranty.
 *In no event will the authors be held liable for any damages arising from
 *the use of this code. Permission is granted to anyone to use this
 *code for any purpose, including commercial applications, and to alter
 *it and redistribute it freely, subject to the following restrictions:
 *
 *1. The origin of this code must not be misrepresented; you must not
 *claim that you wrote the original code. If you use this code in a
 *product, an acknowledgment in the product documentation would be
 *appreciated but is not required.
 *
 *2. Altered source versions must be plainly marked as such, and must not be
 *misrepresented as being the original code.
 *
 *3. This notice may not be removed or altered from any source distribution.
 */
#include "magicmoves.hpp"
#include "bitboard.h"
// altered source 

U64 magicmovesbdb[5248];
const U64 *magicmoves_b_indices[64] =
{
    magicmovesbdb + 4992, magicmovesbdb + 2624, magicmovesbdb + 256, magicmovesbdb + 896,
    magicmovesbdb + 1280, magicmovesbdb + 1664, magicmovesbdb + 4800, magicmovesbdb + 5120,
    magicmovesbdb + 2560, magicmovesbdb + 2656, magicmovesbdb + 288, magicmovesbdb + 928,
    magicmovesbdb + 1312, magicmovesbdb + 1696, magicmovesbdb + 4832, magicmovesbdb + 4928,
    magicmovesbdb + 0, magicmovesbdb + 128, magicmovesbdb + 320, magicmovesbdb + 960,
    magicmovesbdb + 1344, magicmovesbdb + 1728, magicmovesbdb + 2304, magicmovesbdb + 2432,
    magicmovesbdb + 32, magicmovesbdb + 160, magicmovesbdb + 448, magicmovesbdb + 2752,
    magicmovesbdb + 3776, magicmovesbdb + 1856, magicmovesbdb + 2336, magicmovesbdb + 2464,
    magicmovesbdb + 64, magicmovesbdb + 192, magicmovesbdb + 576, magicmovesbdb + 3264,
    magicmovesbdb + 4288, magicmovesbdb + 1984, magicmovesbdb + 2368, magicmovesbdb + 2496,
    magicmovesbdb + 96, magicmovesbdb + 224, magicmovesbdb + 704, magicmovesbdb + 1088,
    magicmovesbdb + 1472, magicmovesbdb + 2112, magicmovesbdb + 2400, magicmovesbdb + 2528,
    magicmovesbdb + 2592, magicmovesbdb + 2688, magicmovesbdb + 832, magicmovesbdb + 1216,
    magicmovesbdb + 1600, magicmovesbdb + 2240, magicmovesbdb + 4864, magicmovesbdb + 4960,
    magicmovesbdb + 5056, magicmovesbdb + 2720, magicmovesbdb + 864, magicmovesbdb + 1248,
    magicmovesbdb + 1632, magicmovesbdb + 2272, magicmovesbdb + 4896, magicmovesbdb + 5184
};

U64 magicmovesrdb[102400];
const U64 *magicmoves_r_indices[64] =
{
    magicmovesrdb + 86016, magicmovesrdb + 73728, magicmovesrdb + 36864, magicmovesrdb + 43008,
    magicmovesrdb + 47104, magicmovesrdb + 51200, magicmovesrdb + 77824, magicmovesrdb + 94208,
    magicmovesrdb + 69632, magicmovesrdb + 32768, magicmovesrdb + 38912, magicmovesrdb + 10240,
    magicmovesrdb + 14336, magicmovesrdb + 53248, magicmovesrdb + 57344, magicmovesrdb + 81920,
    magicmovesrdb + 24576, magicmovesrdb + 33792, magicmovesrdb + 6144, magicmovesrdb + 11264,
    magicmovesrdb + 15360, magicmovesrdb + 18432, magicmovesrdb + 58368, magicmovesrdb + 61440,
    magicmovesrdb + 26624, magicmovesrdb + 4096, magicmovesrdb + 7168, magicmovesrdb + 0,
    magicmovesrdb + 2048, magicmovesrdb + 19456, magicmovesrdb + 22528, magicmovesrdb + 63488,
    magicmovesrdb + 28672, magicmovesrdb + 5120, magicmovesrdb + 8192, magicmovesrdb + 1024,
    magicmovesrdb + 3072, magicmovesrdb + 20480, magicmovesrdb + 23552, magicmovesrdb + 65536,
    magicmovesrdb + 30720, magicmovesrdb + 34816, magicmovesrdb + 9216, magicmovesrdb + 12288,
    magicmovesrdb + 16384, magicmovesrdb + 21504, magicmovesrdb + 59392, magicmovesrdb + 67584,
    magicmovesrdb + 71680, magicmovesrdb + 35840, magicmovesrdb + 39936, magicmovesrdb + 13312,
    magicmovesrdb + 17408, magicmovesrdb + 54272, magicmovesrdb + 60416, magicmovesrdb + 83968,
    magicmovesrdb + 90112, magicmovesrdb + 75776, magicmovesrdb + 40960, magicmovesrdb + 45056,
    magicmovesrdb + 49152, magicmovesrdb + 55296, magicmovesrdb + 79872, magicmovesrdb + 98304
};

U64 initmagicmoves_occ(const int *squares, const int numSquares, const U64 linocc)
{
    int i;
    U64 ret = 0;
    for (i = 0; i < numSquares; i++)
        if (linocc & (((U64)(1)) << i))
            ret |= (((U64)(1)) << squares[i]);
    return ret;
}

U64 initmagicmoves_Rmoves(const int square, const U64 occ)
{
    U64 ret = 0;
    U64 bit;
    U64 rowbits = (((U64)0xFF) << (8 * (square / 8)));

    bit = (((U64)(1)) << square);
    do
    {
        bit <<= 8;
        ret |= bit;
    } while (bit && !(bit & occ));
    bit = (((U64)(1)) << square);
    do
    {
        bit >>= 8;
        ret |= bit;
    } while (bit && !(bit & occ));
    bit = (((U64)(1)) << square);
    do
    {
        bit <<= 1;
        if (bit & rowbits)
            ret |= bit;
        else
            break;
    } while (!(bit & occ));
    bit = (((U64)(1)) << square);
    do
    {
        bit >>= 1;
        if (bit & rowbits)
            ret |= bit;
        else
            break;
    } while (!(bit & occ));
    return ret;
}

U64 initmagicmoves_Bmoves(const int square, const U64 occ)
{
    U64 ret = 0;
    U64 bit;
    U64 bit2;
    U64 rowbits = (((U64)0xFF) << (8 * (square / 8)));

    bit = (((U64)(1)) << square);
    bit2 = bit;
    do
    {
        bit <<= 8 - 1;
        bit2 >>= 1;
        if (bit2 & rowbits)
            ret |= bit;
        else
            break;
    } while (bit && !(bit & occ));
    bit = (((U64)(1)) << square);
    bit2 = bit;
    do
    {
        bit <<= 8 + 1;
        bit2 <<= 1;
        if (bit2 & rowbits)
            ret |= bit;
        else
            break;
    } while (bit && !(bit & occ));
    bit = (((U64)(1)) << square);
    bit2 = bit;
    do
    {
        bit >>= 8 - 1;
        bit2 <<= 1;
        if (bit2 & rowbits)
            ret |= bit;
        else
            break;
    } while (bit && !(bit & occ));
    bit = (((U64)(1)) << square);
    bit2 = bit;
    do
    {
        bit >>= 8 + 1;
        bit2 >>= 1;
        if (bit2 & rowbits)
            ret |= bit;
        else
            break;
    } while (bit && !(bit & occ));
    return ret;
}

#define BmagicNOMASK2(square, occupancy) *(magicmoves_b_indices2[square] + (((occupancy)*magicmoves_b_magics[square]) >> magicmoves_b_shift[square]))
#define RmagicNOMASK2(square, occupancy) *(magicmoves_r_indices2[square] + (((occupancy)*magicmoves_r_magics[square]) >> magicmoves_r_shift[square]))

void initmagicmoves(void)
{
    int i;

    U64 *magicmoves_b_indices2[64] =
    {
        magicmovesbdb + 4992, magicmovesbdb + 2624, magicmovesbdb + 256, magicmovesbdb + 896,
        magicmovesbdb + 1280, magicmovesbdb + 1664, magicmovesbdb + 4800, magicmovesbdb + 5120,
        magicmovesbdb + 2560, magicmovesbdb + 2656, magicmovesbdb + 288, magicmovesbdb + 928,
        magicmovesbdb + 1312, magicmovesbdb + 1696, magicmovesbdb + 4832, magicmovesbdb + 4928,
        magicmovesbdb + 0, magicmovesbdb + 128, magicmovesbdb + 320, magicmovesbdb + 960,
        magicmovesbdb + 1344, magicmovesbdb + 1728, magicmovesbdb + 2304, magicmovesbdb + 2432,
        magicmovesbdb + 32, magicmovesbdb + 160, magicmovesbdb + 448, magicmovesbdb + 2752,
        magicmovesbdb + 3776, magicmovesbdb + 1856, magicmovesbdb + 2336, magicmovesbdb + 2464,
        magicmovesbdb + 64, magicmovesbdb + 192, magicmovesbdb + 576, magicmovesbdb + 3264,
        magicmovesbdb + 4288, magicmovesbdb + 1984, magicmovesbdb + 2368, magicmovesbdb + 2496,
        magicmovesbdb + 96, magicmovesbdb + 224, magicmovesbdb + 704, magicmovesbdb + 1088,
        magicmovesbdb + 1472, magicmovesbdb + 2112, magicmovesbdb + 2400, magicmovesbdb + 2528,
        magicmovesbdb + 2592, magicmovesbdb + 2688, magicmovesbdb + 832, magicmovesbdb + 1216,
        magicmovesbdb + 1600, magicmovesbdb + 2240, magicmovesbdb + 4864, magicmovesbdb + 4960,
        magicmovesbdb + 5056, magicmovesbdb + 2720, magicmovesbdb + 864, magicmovesbdb + 1248,
        magicmovesbdb + 1632, magicmovesbdb + 2272, magicmovesbdb + 4896, magicmovesbdb + 5184
    };

    U64 *magicmoves_r_indices2[64] 
    {
        magicmovesrdb + 86016, magicmovesrdb + 73728, magicmovesrdb + 36864, magicmovesrdb + 43008,
        magicmovesrdb + 47104, magicmovesrdb + 51200, magicmovesrdb + 77824, magicmovesrdb + 94208,
        magicmovesrdb + 69632, magicmovesrdb + 32768, magicmovesrdb + 38912, magicmovesrdb + 10240,
        magicmovesrdb + 14336, magicmovesrdb + 53248, magicmovesrdb + 57344, magicmovesrdb + 81920,
        magicmovesrdb + 24576, magicmovesrdb + 33792, magicmovesrdb + 6144, magicmovesrdb + 11264,
        magicmovesrdb + 15360, magicmovesrdb + 18432, magicmovesrdb + 58368, magicmovesrdb + 61440,
        magicmovesrdb + 26624, magicmovesrdb + 4096, magicmovesrdb + 7168, magicmovesrdb + 0,
        magicmovesrdb + 2048, magicmovesrdb + 19456, magicmovesrdb + 22528, magicmovesrdb + 63488,
        magicmovesrdb + 28672, magicmovesrdb + 5120, magicmovesrdb + 8192, magicmovesrdb + 1024,
        magicmovesrdb + 3072, magicmovesrdb + 20480, magicmovesrdb + 23552, magicmovesrdb + 65536,
        magicmovesrdb + 30720, magicmovesrdb + 34816, magicmovesrdb + 9216, magicmovesrdb + 12288,
        magicmovesrdb + 16384, magicmovesrdb + 21504, magicmovesrdb + 59392, magicmovesrdb + 67584,
        magicmovesrdb + 71680, magicmovesrdb + 35840, magicmovesrdb + 39936, magicmovesrdb + 13312,
        magicmovesrdb + 17408, magicmovesrdb + 54272, magicmovesrdb + 60416, magicmovesrdb + 83968,
        magicmovesrdb + 90112, magicmovesrdb + 75776, magicmovesrdb + 40960, magicmovesrdb + 45056,
        magicmovesrdb + 49152, magicmovesrdb + 55296, magicmovesrdb + 79872, magicmovesrdb + 98304
    };


    for (i = 0; i < 64; i++)
    {
        int squares[64];
        int numsquares = 0;
        U64 temp = magicmoves_b_mask[i];
        while (temp)
            squares[numsquares++] = pop_lsb(temp);

        for (temp = 0; temp < (((U64)(1)) << numsquares); temp++)
        {
            U64 tempocc = initmagicmoves_occ(squares, numsquares, temp);
            BmagicNOMASK2(i, tempocc) = initmagicmoves_Bmoves(i, tempocc);
        }
    }
    
    for (i = 0; i < 64; i++)
    {
        int squares[64];
        int numsquares = 0;
        U64 temp = magicmoves_r_mask[i];
        while (temp)
            squares[numsquares++] = pop_lsb(temp);

        for (temp = 0; temp < (((U64)(1)) << numsquares); temp++)
        {
            U64 tempocc = initmagicmoves_occ(squares, numsquares, temp);
            RmagicNOMASK2(i, tempocc) = initmagicmoves_Rmoves(i, tempocc);
        }
    }
}

void initMagics()
{
    initmagicmoves();
}