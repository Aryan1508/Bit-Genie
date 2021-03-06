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
  hash ^= piece_keys[to_int(get_piece_type(piece))][to_int(get_piece_color(piece))][to_int(sq)];
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
      piece_keys[i][to_int(Color::white)][j] = dist(gen);
      piece_keys[i][to_int(Color::black)][j] = dist(gen);
    }
  }
  printf("done.\n");
}

void ZobristKey::hash_pieces(Position const& position)
{
  for (Square sq = Square::A1;sq <= Square::H8;sq++)
  {
    const Piece piece = position.pieces.get_piece(sq);
    if (piece != Piece::empty)
    {
      hash_piece(sq, piece);
    }
  }
}

void ZobristKey::hash_ep(const Square sq)
{
  assert(is_ok(sq));
  hash ^= enpassant_keys[to_int(get_square_file(sq))];
}

void ZobristKey::generate(Position const& position)
{
  // Reset the key to 0 to hash in new information
  reset();

  hash_pieces(position);
  hash_castle(CastleRights(), position.castle_rights);

  if (position.player() == Color::white)
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