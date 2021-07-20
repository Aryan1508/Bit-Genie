/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
    uint64_t piece_keys[12][64];
    uint64_t enpassant_keys[8];
    uint64_t castle_keys[64];

    uint64_t color_key;
}

ZobristKey::ZobristKey()
    : hash(0)
{
}

void ZobristKey::hash_side()
{
    hash ^= color_key;
}

void ZobristKey::hash_piece(Square sq, Piece piece)
{
    hash ^= piece_keys[piece][sq];
}

void ZobristKey::hash_castle(uint64_t old, const uint64_t updated)
{
    uint64_t removed_rooks = old ^ updated;
    while (removed_rooks)
    {
        Square removed_rook = pop_lsb(removed_rooks);
        hash ^= castle_keys[removed_rook];
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

    for (int i = 0; i < 8; i++)
        enpassant_keys[i] = dist(gen);

    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            castle_keys[j] = dist(gen);
            piece_keys[i][j] = dist(gen);
            piece_keys[i][j] = dist(gen);
        }
    }
}

void ZobristKey::hash_pieces(Position const &position)
{
    for (Square sq = Square::A1; sq <= Square::H8; sq++)
    {
        Piece piece = position.get_piece(sq);
        if (piece != Empty)
        {
            hash_piece(sq, piece);
        }
    }
}

void ZobristKey::hash_ep(const Square sq)
{
    hash ^= enpassant_keys[to_int(file_of(sq))];
}

void ZobristKey::generate(Position const &position)
{
    reset();

    hash_pieces(position);
    hash_castle(0, position.get_castle_rooks());

    if (position.get_side() == White)
    {
        hash_side();
    }

    if (position.get_ep() != Square::bad_sq)
    {
        hash_ep(position.get_ep());
    }
}

std::ostream &operator<<(std::ostream &o, const ZobristKey key)
{
    return o << std::hex << "0x" << key.hash << std::dec;
}