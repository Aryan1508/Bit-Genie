#include <array>
#include "board.h"
#include "bitboard.h"
#include <iostream>
#include "misc.h"
#include "piece.h"
#include "position.h"
#include <random>
#include "Square.h"
#include "zobrist.h"

namespace
{
  uint64_t piece_keys[total_pieces * total_colors][total_squares];
  uint64_t enpassant_keys[total_files];
  uint64_t castle_keys[total_squares];

  uint64_t color_key;
}

ZobristKey::ZobristKey()
  : hash(0)
{}

void ZobristKey::hash_side()
{
  hash ^= color_key;
}

void ZobristKey::hash_piece(Square sq, Piece piece)
{
  hash ^= piece_keys[piece][sq];
}

void ZobristKey::hash_castle(const CastleRights old_rooks, const CastleRights new_rooks)
{
  uint64_t removed_rooks = old_rooks.data() ^ new_rooks.data();
  while (removed_rooks) 
  {
    Square removed_rook = pop_lsb(removed_rooks);
    hash ^= castle_keys[to_int(removed_rook)];
  }
}

void ZobristKey::reset()
{
  hash = 0;
}

void ZobristKey::init()
{
  std::mt19937 gen(0);
  std::uniform_int_distribution<uint64_t> dist(10, std::numeric_limits<uint64_t>::max());

  color_key = dist(gen);
  
  for (int i = 0; i < total_files; i++)
    enpassant_keys[i] = dist(gen);

  for (int i = 0; i < 12; i++) 
  {
    for (int j = 0; j < total_squares; j++)
    {
      castle_keys[j] = dist(gen);
      piece_keys[i][j] = dist(gen);
      piece_keys[i][j] = dist(gen);
    }
  }
}

void ZobristKey::hash_pieces(Position const& position)
{
  for (Square sq = Square::A1;sq <= Square::H8;sq++)
  {
    const Piece piece = position.pieces.get_piece(sq);
    if (piece != Empty)
    {
      hash_piece(sq, piece);
    }
  }
}

void ZobristKey::hash_ep(const Square sq)
{
  assert(is_ok(sq));
  hash ^= enpassant_keys[to_int(file_of(sq))];
}

void ZobristKey::generate(Position const& position)
{
  reset();

  hash_pieces(position);
  hash_castle(CastleRights(), position.castle_rights);

  if (position.side == White)
  {
    hash_side();
  }

  if (position.ep_sq != Square::bad_sq)
  {
    hash_ep(position.ep_sq);
  }
}

std::ostream& operator<<(std::ostream& o, const ZobristKey key)
{
  return o << std::hex << "0x" << key.hash << std::dec;
}