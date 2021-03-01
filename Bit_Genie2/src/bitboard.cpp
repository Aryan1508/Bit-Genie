#include "bitboard.h"
#include "Square.h"
#include "board.h"
#include <bitset>
#if defined(_MSC_VER)
#include <intrin.h>
#endif

static inline void clear_lsb(uint64_t& bits) {
  bits &= (bits - 1);
}

static void print_bit(std::ostream& o, bool bit) {
  if (bit) o << "1 ";
  else o << ". ";
}

Bitboard::Bitboard()
  : bits(0)
{}

Bitboard::Bitboard(const uint64_t data)
  : bits(data) 
{}

Bitboard::Bitboard(const Square sq)
  : bits(1ull << to_underlying(sq))
{}

Bitboard::operator bool() const {
  return bits != 0;
}

int Bitboard::popcnt() const {
  return static_cast<int>(std::bitset<64>(bits).count());
}

bool Bitboard::test_bit(const Square sq) const {
  return (bits & (1ull << to_underlying(sq)));
}

std::ostream& operator<<(std::ostream& o, Bitboard bb) {
  for (Square sq = Square::A1; sq <= Square::H8;sq++) {
    if (to_underlying(sq) % 8 == 0)
      o << '\n';

    print_bit(o, bb.test_bit(flip_square(sq)));
  }
  return o;
}

Square Bitboard::get_lsb() const {
  assert(bits);
#if defined(_MSC_VER)
  unsigned long ind;
  _BitScanForward64(&ind, bits);
#elif defined(__GNUC__)
  unsigned long ind = __builtin_ctzll(bb);
#endif
  return static_cast<Square>(ind);
}

Square Bitboard::pop_lsb() {
  assert(bits);
  const Square index = get_lsb();
  clear_lsb(bits);
  return index;
}

bool Bitboard::has_multiple() const {
  uint64_t temp = bits;
  clear_lsb(temp);

  // If number of bits was > 1, clearing the lsb 
  // would leave us with non-zero value. 
  return temp != 0;
}

Bitboard Bitboard::reverse_bytes() const {
#if defined(_MSC_VER)
  return _byteswap_uint64(bits);
#elif defined(__GNUC__)
  return  __builtin_bswap64(bits);
#endif
}