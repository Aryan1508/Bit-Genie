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
#include "types.h"

#include "magicmoves.hpp"

constexpr std::uint64_t FILE_A_BB = 0x0101010101010101;
constexpr std::uint64_t FILE_B_BB = FILE_A_BB << 1;
constexpr std::uint64_t FILE_C_BB = FILE_B_BB << 1;
constexpr std::uint64_t FILE_D_BB = FILE_C_BB << 1;
constexpr std::uint64_t FILE_E_BB = FILE_D_BB << 1;
constexpr std::uint64_t FILE_F_BB = FILE_E_BB << 1;
constexpr std::uint64_t FILE_G_BB = FILE_F_BB << 1;
constexpr std::uint64_t FILE_H_BB = FILE_G_BB << 1;

constexpr std::uint64_t RANK_1_BB = 0xff;
constexpr std::uint64_t RANK_2_BB = RANK_1_BB << 8;
constexpr std::uint64_t RANK_3_BB = RANK_2_BB << 8;
constexpr std::uint64_t RANK_4_BB = RANK_3_BB << 8;
constexpr std::uint64_t RANK_5_BB = RANK_4_BB << 8;
constexpr std::uint64_t RANK_6_BB = RANK_5_BB << 8;
constexpr std::uint64_t RANK_7_BB = RANK_6_BB << 8;
constexpr std::uint64_t RANK_8_BB = RANK_7_BB << 8;

constexpr std::uint64_t KNIGHT_ATTACK_BB_LOOKUP[64]{
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

constexpr std::uint64_t KING_ATTACK_BB_LOOKUP[64]{
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

// clang-format off

constexpr auto shift(const std::uint64_t bits, const Direction dir) {
    assert(is_ok(dir));
    return  dir == DIR_NORTH ?    bits << 8ull : 
            dir == DIR_SOUTH ?    bits >> 8ull
         :  dir == DIR_EAST  ?   (bits << 1ull) & ~FILE_A_BB
         :/*dir == DIR_WEST  ?*/ (bits >> 1ull) & ~FILE_H_BB;
}

// clang-format on

constexpr auto get_lsb(const std::uint64_t bb) {
    assert(bb != 0);
    return static_cast<Square>(__builtin_ctzll(bb));
}

constexpr auto popcount64(const std::uint64_t bb) {
    return static_cast<std::uint8_t>(__builtin_popcountll(bb));
}

constexpr auto pop_lsb(std::uint64_t &bb) {
    const auto index = get_lsb(bb);
    bb &= (bb - 1);
    return index;
}

constexpr bool test_bit(const std::uint64_t bb, const Square sq) {
    return (1ull << sq) & bb;
}

constexpr void set_bit(std::uint64_t &bb, const Square sq) {
    bb |= (1ull << sq);
}

constexpr void flip_bit(std::uint64_t &bb, const Square sq) {
    bb ^= (1ull << sq);
}

constexpr bool is_several(const std::uint64_t bb) {
    return bb & (bb - 1);
}

constexpr auto compute_knight_attack_bb(const Square sq) {
    assert(is_ok(sq));
    return KNIGHT_ATTACK_BB_LOOKUP[sq];
}

inline auto compute_bishop_attack_bb(const Square sq, const std::uint64_t occupancy) {
    assert(is_ok(sq));
    return Bmagic(sq, occupancy);
}

inline auto compute_rook_attack_bb(const Square sq, const std::uint64_t occupancy) {
    assert(is_ok(sq));
    return Rmagic(sq, occupancy);
}

inline auto compute_queen_attack_bb(const Square sq, const std::uint64_t occupancy) {
    assert(is_ok(sq));
    return Qmagic(sq, occupancy);
}

constexpr auto compute_king_attack_bb(const Square sq) {
    assert(is_ok(sq));
    return KING_ATTACK_BB_LOOKUP[sq];
}

constexpr auto compute_pawn_attack_bb(const Square sq, const Color clr) {
    assert(is_ok(sq) && is_ok(clr));
    const auto forward = shift(1ull << sq, compute_relative_forward(clr));
    return shift(forward, DIR_EAST) | shift(forward, DIR_WEST);
}

constexpr auto get_castling_path(const Square sq) {
    assert(sq == SQ_C1 || sq == SQ_G1 || sq == SQ_C8 || sq == SQ_G8);
    // clang-format off
    return sq == SQ_C1 ? 0x0000000000000000000E : 
           sq == SQ_G1 ? 0x00000000000000000060 : 
           sq == SQ_C8 ? 0x00000e00000000000000 : 0x00006000000000000000;
}

constexpr auto compute_attack_bb(const PieceType piece, const Square sq, const std::uint64_t occ) {
    switch (piece) {
    case PT_KNIGHT:
        return compute_knight_attack_bb(sq);
    case PT_BISHOP:
        return compute_bishop_attack_bb(sq, occ);
    case PT_ROOK:
        return compute_rook_attack_bb(sq, occ);
    case PT_QUEEN:
        return compute_queen_attack_bb(sq, occ);
    default:
        return compute_king_attack_bb(sq);
    }
}