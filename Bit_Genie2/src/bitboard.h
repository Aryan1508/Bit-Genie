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

  if (b & 0xffffffff) {
    _BitScanForward(&idx, int32_t(b));
    return Square(idx);
  }
  else {
    _BitScanForward(&idx, int32_t(b >> 32));
    return Square(idx + 32);
  }
}

#else
inline Square get_lsb(uint64_t bb)
{
  assert(bb);
  return __builtin_ctzll(bb);
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
  return (1ull << to_int(sq)) & bb;
}

inline void set_bit(Square sq, uint64_t& bb)
{
  bb |= (1ull << to_int(sq));
}

inline void print_uint64_t(uint64_t bb)
{
  for (Square sq = Square::A1; sq <= Square::H8; sq++)
  {
    if (to_int(sq) % 8 == 0)
      std::cout << '\n';

    if (test_bit(sq, bb))
      std::cout << "1 ";

    else
      std::cout << ". ";

  }
}