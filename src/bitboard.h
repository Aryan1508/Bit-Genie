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
#include "board.h"

constexpr uint64_t FILE_A_BB = 0x0101010101010101;
constexpr uint64_t FILE_B_BB = FILE_A_BB << 1;
constexpr uint64_t FILE_C_BB = FILE_B_BB << 1;
constexpr uint64_t FILE_D_BB = FILE_C_BB << 1;
constexpr uint64_t FILE_E_BB = FILE_D_BB << 1;
constexpr uint64_t FILE_F_BB = FILE_E_BB << 1;
constexpr uint64_t FILE_G_BB = FILE_F_BB << 1;
constexpr uint64_t FILE_H_BB = FILE_G_BB << 1;

constexpr uint64_t RANK_1_BB = 0xff;
constexpr uint64_t RANK_2_BB = RANK_1_BB << 8;
constexpr uint64_t RANK_3_BB = RANK_2_BB << 8;
constexpr uint64_t RANK_4_BB = RANK_3_BB << 8;
constexpr uint64_t RANK_5_BB = RANK_4_BB << 8;
constexpr uint64_t RANK_6_BB = RANK_5_BB << 8;
constexpr uint64_t RANK_7_BB = RANK_6_BB << 8;
constexpr uint64_t RANK_8_BB = RANK_7_BB << 8;

constexpr uint64_t KNIGHT_ATTACKS_BB[64]{
    0X0000000000020400, 0X0000000000050800, 0X00000000000a1100, 0X0000000000142200,
    0X0000000000284400, 0X0000000000508800, 0X0000000000a01000, 0X0000000000402000,
    0X0000000002040004, 0X0000000005080008, 0X000000000a110011, 0X0000000014220022,
    0X0000000028440044, 0X0000000050880088, 0X00000000a0100010, 0X0000000040200020,
    0X0000000204000402, 0X0000000508000805, 0X0000000a1100110a, 0X0000001422002214,
    0X0000002844004428, 0X0000005088008850, 0X000000a0100010a0, 0X0000004020002040,
    0X0000020400040200, 0X0000050800080500, 0X00000a1100110a00, 0X0000142200221400,
    0X0000284400442800, 0X0000508800885000, 0X0000a0100010a000, 0X0000402000204000,
    0X0002040004020000, 0X0005080008050000, 0X000a1100110a0000, 0X0014220022140000,
    0X0028440044280000, 0X0050880088500000, 0X00a0100010a00000, 0X0040200020400000,
    0X0204000402000000, 0X0508000805000000, 0X0a1100110a000000, 0X1422002214000000,
    0X2844004428000000, 0X5088008850000000, 0Xa0100010a0000000, 0X4020002040000000,
    0X0400040200000000, 0X0800080500000000, 0X1100110a00000000, 0X2200221400000000,
    0X4400442800000000, 0X8800885000000000, 0X100010a000000000, 0X2000204000000000,
    0X0004020000000000, 0X0008050000000000, 0X00110a0000000000, 0X0022140000000000,
    0X0044280000000000, 0X0088500000000000, 0X0010a00000000000, 0X0020400000000000
};

constexpr uint64_t KING_ATTACKS_BB[64]{
    0X0000000000000302, 0X0000000000000705, 0X0000000000000e0a, 0X0000000000001c14,
    0X0000000000003828, 0X0000000000007050, 0X000000000000e0a0, 0X000000000000c040,
    0X0000000000030203, 0X0000000000070507, 0X00000000000e0a0e, 0X00000000001c141c,
    0X0000000000382838, 0X0000000000705070, 0X0000000000e0a0e0, 0X0000000000c040c0,
    0X0000000003020300, 0X0000000007050700, 0X000000000e0a0e00, 0X000000001c141c00,
    0X0000000038283800, 0X0000000070507000, 0X00000000e0a0e000, 0X00000000c040c000,
    0X0000000302030000, 0X0000000705070000, 0X0000000e0a0e0000, 0X0000001c141c0000,
    0X0000003828380000, 0X0000007050700000, 0X000000e0a0e00000, 0X000000c040c00000,
    0X0000030203000000, 0X0000070507000000, 0X00000e0a0e000000, 0X00001c141c000000,
    0X0000382838000000, 0X0000705070000000, 0X0000e0a0e0000000, 0X0000c040c0000000,
    0X0003020300000000, 0X0007050700000000, 0X000e0a0e00000000, 0X001c141c00000000,
    0X0038283800000000, 0X0070507000000000, 0X00e0a0e000000000, 0X00c040c000000000,
    0X0302030000000000, 0X0705070000000000, 0X0e0a0e0000000000, 0X1c141c0000000000,
    0X3828380000000000, 0X7050700000000000, 0Xe0a0e00000000000, 0Xc040c00000000000,
    0X0203000000000000, 0X0507000000000000, 0X0a0e000000000000, 0X141c000000000000,
    0X2838000000000000, 0X5070000000000000, 0Xa0e0000000000000, 0X40c0000000000000
};

constexpr uint64_t PAWN_ATTACKS_BB[2][64]{
    { 0X000000000000000200, 0X000000000000000500, 0X000000000000000a00, 0X000000000000001400,
      0X000000000000002800, 0X000000000000005000, 0X00000000000000a000, 0X000000000000004000,
      0X000000000000020000, 0X000000000000050000, 0X0000000000000a0000, 0X000000000000140000,
      0X000000000000280000, 0X000000000000500000, 0X000000000000a00000, 0X000000000000400000,
      0X000000000002000000, 0X000000000005000000, 0X00000000000a000000, 0X000000000014000000,
      0X000000000028000000, 0X000000000050000000, 0X0000000000a0000000, 0X000000000040000000,
      0X000000000200000000, 0X000000000500000000, 0X000000000a00000000, 0X000000001400000000,
      0X000000002800000000, 0X000000005000000000, 0X00000000a000000000, 0X000000004000000000,
      0X000000020000000000, 0X000000050000000000, 0X0000000a0000000000, 0X000000140000000000,
      0X000000280000000000, 0X000000500000000000, 0X000000a00000000000, 0X000000400000000000,
      0X000002000000000000, 0X000005000000000000, 0X00000a000000000000, 0X000014000000000000,
      0X000028000000000000, 0X000050000000000000, 0X0000a0000000000000, 0X000040000000000000,
      0X000200000000000000, 0X000500000000000000, 0X000a00000000000000, 0X001400000000000000,
      0X002800000000000000, 0X005000000000000000, 0X00a000000000000000, 0X004000000000000000,
      0X000000000000000000, 0X000000000000000000, 0X000000000000000000, 0X000000000000000000,
      0X000000000000000000, 0X000000000000000000, 0X000000000000000000, 0X000000000000000000 },
    { 0X000000000000000000, 0X000000000000000000, 0X000000000000000000, 0X000000000000000000,
      0X000000000000000000, 0X000000000000000000, 0X000000000000000000, 0X000000000000000000,
      0X000000000000000002, 0X000000000000000005, 0X00000000000000000a, 0X000000000000000014,
      0X000000000000000028, 0X000000000000000050, 0X0000000000000000a0, 0X000000000000000040,
      0X000000000000000200, 0X000000000000000500, 0X000000000000000a00, 0X000000000000001400,
      0X000000000000002800, 0X000000000000005000, 0X00000000000000a000, 0X000000000000004000,
      0X000000000000020000, 0X000000000000050000, 0X0000000000000a0000, 0X000000000000140000,
      0X000000000000280000, 0X000000000000500000, 0X000000000000a00000, 0X000000000000400000,
      0X000000000002000000, 0X000000000005000000, 0X00000000000a000000, 0X000000000014000000,
      0X000000000028000000, 0X000000000050000000, 0X0000000000a0000000, 0X000000000040000000,
      0X000000000200000000, 0X000000000500000000, 0X000000000a00000000, 0X000000001400000000,
      0X000000002800000000, 0X000000005000000000, 0X00000000a000000000, 0X000000004000000000,
      0X000000020000000000, 0X000000050000000000, 0X0000000a0000000000, 0X000000140000000000,
      0X000000280000000000, 0X000000500000000000, 0X000000a00000000000, 0X000000400000000000,
      0X000002000000000000, 0X000005000000000000, 0X00000a000000000000, 0X000014000000000000,
      0X000028000000000000, 0X000050000000000000, 0X0000a0000000000000, 0X000040000000000000 }
};

constexpr uint64_t ADJ_FILES_BB[64]{
    0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828,
    0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040, 0X00202020202020202, 0X00505050505050505,
    0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0,
    0X04040404040404040, 0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414,
    0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040, 0X00202020202020202,
    0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828, 0X05050505050505050,
    0X0a0a0a0a0a0a0a0a0, 0X04040404040404040, 0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a,
    0X01414141414141414, 0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040,
    0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828,
    0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040, 0X00202020202020202, 0X00505050505050505,
    0X00a0a0a0a0a0a0a0a, 0X01414141414141414, 0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0,
    0X04040404040404040, 0X00202020202020202, 0X00505050505050505, 0X00a0a0a0a0a0a0a0a, 0X01414141414141414,
    0X02828282828282828, 0X05050505050505050, 0X0a0a0a0a0a0a0a0a0, 0X04040404040404040
};

constexpr uint64_t CASTLE_MOVE_UPDATES_BB[64]{
    0x004400000000000040, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000000, 0x004400000000000044, 0x004400000000000044, 0x004400000000000004,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004000000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x000000000000000044, 0x004400000000000044, 0x004400000000000044, 0x000400000000000044
};

constexpr uint64_t CASTLE_CHECKS_MASK_BB[64]{
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000008, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000020, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000800000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00002000000000000000, 0x0000000000000000000
};

constexpr uint64_t CASTLE_OCC_MASK_BB[64]{
    0x000000000000000000, 0x00000000000000000000, 0x0000000000000000000E, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000060, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000000000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00000e00000000000000, 0x0000000000000000000,
    0x000000000000000000, 0x00000000000000000000, 0x00006000000000000000, 0x0000000000000000000
};

template <Direction dir>
constexpr uint64_t shift(uint64_t bits) noexcept {
    return dir == DIR_NORTH   ? bits << 8
           : dir == DIR_SOUTH ? bits >> 8
           : dir == DIR_EAST  ? (bits << 1) & ~FILE_A_BB
                              : (bits >> 1) & ~FILE_H_BB;
}

constexpr uint64_t shift(uint64_t bits, Direction dir) noexcept {
    return dir == DIR_NORTH   ? bits << 8
           : dir == DIR_SOUTH ? bits >> 8
           : dir == DIR_EAST  ? (bits << 1) & ~FILE_A_BB
                              : (bits >> 1) & ~FILE_H_BB;
}

inline Square get_lsb(uint64_t bb) noexcept {
    return static_cast<Square>(__builtin_ctzll(bb));
}

inline int popcount64(uint64_t bb) noexcept {
    return static_cast<int>(__builtin_popcountll(bb));
}

inline Square pop_lsb(uint64_t &bb) noexcept {
    Square index = get_lsb(bb);
    bb &= (bb - 1);
    return index;
}

constexpr bool test_bit(uint64_t bb, Square sq) noexcept {
    return (1ull << sq) & bb;
}

constexpr void set_bit(uint64_t &bb, Square sq) noexcept {
    bb |= (1ull << sq);
}

constexpr void flip_bit(uint64_t &bb, Square sq) noexcept {
    bb ^= (1ull << sq);
}

constexpr bool is_several(uint64_t bb) noexcept {
    return bb & (bb - 1);
}