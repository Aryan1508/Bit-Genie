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
  //              piece y of color x on square sq
  // indexed by [piece_type][piece_color][square]
  uint64_t piece_keys[total_pieces][total_colors][total_squares];
  
  // indexed by [get_square_file(enpassant_square)]
  uint64_t enpassant_keys[total_files];

  // indexed by [castle_type][color]
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
  hash ^= piece_keys[piece.get_type()][piece.get_color()][to_int(sq)];
}

void ZobristKey::hash_castle(const CastleRights old_rooks, const CastleRights new_rooks)
{
  Bitboard removed_rooks = old_rooks.data() ^ new_rooks.data();
  while (removed_rooks) 
  {
    const Square removed_rook = removed_rooks.pop_lsb();
    hash ^= castle_keys[to_int(removed_rook)];
  }
}

void ZobristKey::reset()
{
  hash = 0;
}

void ZobristKey::init()
{
  printf("Initializing Zobrist keys... ");
  std::mt19937 gen(0);
  std::uniform_int_distribution<uint64_t> dist(10, std::numeric_limits<uint64_t>::max());

  color_key = dist(gen);
  
  for (int i = 0; i < total_files; i++)
    enpassant_keys[i] = dist(gen);

  for (int i = 0; i < total_pieces; i++) 
  {
    for (int j = 0; j < total_squares; j++)
    {
      castle_keys[j] = dist(gen);
      piece_keys[i][Piece::white][j] = dist(gen);
      piece_keys[i][Piece::black][j] = dist(gen);
    }
  }
  printf("done.\n");
}

void ZobristKey::hash_pieces(Position const& position)
{
  for (Square sq = Square::A1;sq <= Square::H8;sq++)
  {
    if (!(position.get_piece(sq).is_empty()))
    {
      hash_piece(sq, position.get_piece(sq));
    }
  }
}

void ZobristKey::hash_ep(const Square sq)
{
  hash ^= enpassant_keys[to_int(get_square_file(sq))];
}

void ZobristKey::generate(Position const& position)
{
  // Reset the key to 0 to hash in new information
  reset();

  hash_pieces(position);
  hash_castle(CastleRights(), position.get_castle_rights());

  if (position.player() == Piece::white)
  {
    hash_side();
  }

  if (position.get_ep() != Square::bad)
  {
    hash_ep(position.get_ep());
  }
}

std::ostream& operator<<(std::ostream& o, const ZobristKey key)
{
  return o << std::hex << key.hash << std::dec;
}