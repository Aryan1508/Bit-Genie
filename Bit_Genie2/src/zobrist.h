#pragma once
#include "misc.h"
class ZobristKey
{
public:
  ZobristKey();
  void generate(Position const&);

  void hash_side();
  void hash_ep(const Square);
  void hash_piece(const Square, const Piece);
  void hash_castle(const CastleRights, const CastleRights);
  void reset();

  uint64_t data() const { return hash; }

  static void init();

  friend std::ostream& operator<<(std::ostream& o, const ZobristKey);
private:
  void hash_pieces(Position const&);

private:
  uint64_t hash;
};