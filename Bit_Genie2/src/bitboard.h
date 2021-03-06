#pragma once
#include "bitmask.h"
#include <iostream>
#include "misc.h"


template<Direction dir>
constexpr uint64_t shift(uint64_t bits) 
{
  return dir == Direction::north ? bits << 8
    :    dir == Direction::south ? bits >> 8
    :    dir == Direction::east  ? (bits << 1) & BitMask::not_file_a
    :    /* west */ (bits >> 1) & BitMask::not_file_h;
}

inline uint64_t shift(uint64_t bits, Direction dir)
{
  return dir == Direction::north ? bits << 8
    : dir == Direction::south ? bits >> 8
    : dir == Direction::east ? (bits << 1) & BitMask::not_file_a
    :    /* west */ (bits >> 1) & BitMask::not_file_h;
}

#if defined (_MSC_VER)
inline Square get_lsb(uint64_t b) {
  assert(b);
  unsigned long idx;
  _BitScanForward64(&idx, b);
  return to_sq(idx);
}

#else
inline Square get_lsb(uint64_t bb)
{
  assert(bb);
  return static_cast<Square>(__builtin_ctzll(bb));
}
#endif // 


inline Square pop_lsb(uint64_t& bb)
{
  assert(bb);
  Square index = get_lsb(bb);
  bb &= (bb - 1);
  return index;
}

inline bool test_bit(Square sq, uint64_t bb)
{
  return (1ull << sq) & bb;
}

inline void set_bit(Square sq, uint64_t& bb)
{
  bb |= (1ull << sq);
}

inline void print_uint64_t(uint64_t bb)
{
  for (Square sq = Square::A1; sq <= Square::H8; sq++)
  {
    if (sq % 8 == 0)
      std::cout << '\n';

    if (test_bit(flip_square(sq), bb))
      std::cout << "1 ";

    else
      std::cout << ". ";

  }
}