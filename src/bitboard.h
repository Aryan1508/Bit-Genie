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
#include "bitmask.h"
#include "Square.h"

template <Direction dir>
constexpr uint64_t shift(uint64_t bits)
{
    return dir == Direction::north   ? bits << 8
           : dir == Direction::south ? bits >> 8
           : dir == Direction::east  ? (bits << 1) & BitMask::not_file_a
                                     : (bits >> 1) & BitMask::not_file_h;
}

constexpr uint64_t shift(uint64_t bits, Direction dir)
{
    return dir == Direction::north   ? bits << 8
           : dir == Direction::south ? bits >> 8
           : dir == Direction::east  ? (bits << 1) & BitMask::not_file_a
                                     : (bits >> 1) & BitMask::not_file_h;
}

inline Square get_lsb(uint64_t bb)
{
    return static_cast<Square>(__builtin_ctzll(bb));
}

inline int popcount64(uint64_t bb)
{
    return static_cast<int>(__builtin_popcountll(bb));
}

inline Square pop_lsb(uint64_t &bb)
{
    Square index = get_lsb(bb);
    bb &= (bb - 1);
    return index;
}

inline bool test_bit(uint64_t bb, Square sq)
{
    return (1ull << sq) & bb;
}

inline void set_bit(uint64_t &bb, Square sq)
{
    bb |= (1ull << sq);
}

inline void flip_bit(uint64_t& bb, Square sq)
{
    bb ^= (1ull << sq);
}

inline bool is_several(uint64_t bb)
{
    return bb & (bb - 1);
}