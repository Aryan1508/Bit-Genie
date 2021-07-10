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

#if defined(_MSC_VER)
// Position of the least significant bit in a bitboard.
inline Square get_lsb(uint64_t b)
{
    unsigned long idx;
    _BitScanForward64(&idx, b);
    return to_sq(idx);
}

// Count of total number of bits set in a bitboard
inline int popcount64(uint64_t bb)
{
    return static_cast<int>(__popcnt64(bb));
}

#else
// Position of the least significant bit in a bitboard
inline Square get_lsb(uint64_t bb)
{
    return static_cast<Square>(__builtin_ctzll(bb));
}

// Count of total number of bits set in a bitboard
inline int popcount64(uint64_t bb)
{
    return static_cast<int>(__builtin_popcountll(bb));
}
#endif

// Pop the least significant bit in a bitboard
// and return its index
inline Square pop_lsb(uint64_t &bb)
{
    Square index = get_lsb(bb);
    bb &= (bb - 1);
    return index;
}

// Check whether the bit at the given position is set
inline bool test_bit(Square sq, uint64_t bb)
{
    return (1ull << sq) & bb;
}

// Set the bit at the given position
inline void set_bit(Square sq, uint64_t &bb)
{
    bb |= (1ull << sq);
}

// Check whether a bitboard has >1 bits set
inline bool is_several(uint64_t bb)
{
    return bb & (bb - 1);
}