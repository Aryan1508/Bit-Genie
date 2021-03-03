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
  void hash_ep(const Square);
  void hash_piece(const Square, const Piece);
  void hash_castle(const CastleRights, const CastleRights);

  // reset the hash to 0
  void reset();

  // Initialize random keys for hashing
  static void init();

  friend std::ostream& operator<<(std::ostream& o, const ZobristKey);
private:
  void hash_pieces(Position const&);

private:
  uint64_t hash;
};