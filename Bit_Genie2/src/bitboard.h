#pragma once
#include "misc.h"
#include "bitmask.h"
#include <iostream>

class Bitboard {
public:
  Bitboard();

  // Create a new bitboard from a new 64-bit number
  Bitboard(const uint64_t);

  // Create a bitboard and only set the given bit
  // indexed by the given square
  // 
  //    . . . . . . . . 
  //    . . . . . . . .
  //    . . . . . . . . 
  //    . . . . . . . .
  //    . . . . . . . .  -> Bitboard(Square::E1)
  //    . . . . . . . .
  //    . . . . . . . . 
  //    . . . . 1 . . .  
  //  
  // 
  explicit Bitboard(const Square);

  
  // returns false if no bits are set(bits == 0), otherwise true
  explicit operator bool() const;

  // Population-count. The total number of bits that are set
  int popcnt() const;

  // Get the index of the least significant bit 
  Square get_lsb() const;

  // Clear the least significant bit and return its index
  Square pop_lsb();

  // Check if number of bits set in bitboard are > 1
  bool has_multiple() const;

  // Shift all the bits in the given direction according to LERF Square mapping
  //
  // Example 1
  //       shift<Direction::north>
  // . . . . . .       . . . . . .
  // . . . . . .  -->  . . . 1 . .
  // . . . 1 . .       . . . . . .
  // 
  //      
  // Example 2
  //       shift<Direction::east>
  //  . . . . . .       . . . . . .           . . . . . . 
  //  . . . . . .  -->  . . . . . .  BUT NOT  1 . . . . .  
  //  . . . . . 1       . . . . . .           . . . . . .
  template<Direction dir>
  Bitboard shift() const {
    return dir == Direction::north ? bits << 8
      :    dir == Direction::south ? bits >> 8
      :    dir == Direction::east  ? (bits << 1) & ~BitMask::file_a
      :    /* west */ (bits >> 1) & ~BitMask::file_h;
  }

  // Check whether the bit at the given square is set
  bool test_bit(const Square) const;

  // Reverse the byte-order of the bitboard
  Bitboard reverse_bytes() const;
 
  // Print the individual bits of the bitboard 
  // in the form of a chess board
  // 
  //  1 1 1 1 1 1 1 1
  //  1 1 1 1 . 1 1 1
  //  . . . . . . . .
  //  . . . . 1 . . .
  //  . . . . 1 . . .
  //  . . . . . 1 . .
  //  1 1 1 1 . 1 1 1
  //  1 1 1 1 1 1 . 1
  friend std::ostream& operator<<(std::ostream&, Bitboard);

public:
  Bitboard operator^(const Bitboard other) const { return bits ^ other.bits; }

private:
  uint64_t bits;
};