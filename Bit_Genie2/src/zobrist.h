#pragma once
#include "misc.h"

// Hash of a chess position 
// https://en.wikipedia.org/wiki/Zobrist_hashing
class ZobristKey
{
public:
  ZobristKey();

  // Construct a new hash from 
  // the given position's details 
  void generate(Position const&);

  void hash_side();
  void hash_piece(const Square, const Piece);
  void hash_castle(const Bitboard old_rooks, const Bitboard new_rooks);

  // reset the hash to 0
  void reset();

  // Initialize random keys for hashing
  static void init();
private:
  uint64_t hash;
};