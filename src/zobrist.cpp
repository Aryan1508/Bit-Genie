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

#include "bitboard.h"
#include <iostream>

#include "position.h"
#include "types.h"
#include "zobrist.h"
#include <random>

namespace {
std::uint64_t piece_keys[12][64];
std::uint64_t enpassant_keys[8];
std::uint64_t castle_keys[64];

std::uint64_t color_key;
} // namespace

ZobristKey::ZobristKey() : hash(0) {
}

void ZobristKey::hash_side() {
    hash ^= color_key;
}

void ZobristKey::hash_piece(Square sq, Piece piece) {
    hash ^= piece_keys[piece][sq];
}

void ZobristKey::hash_castle(std::uint64_t old, const std::uint64_t updated) {
    std::uint64_t removed_rooks = old ^ updated;
    while (removed_rooks) {
        Square removed_rook = pop_lsb(removed_rooks);
        hash ^= castle_keys[removed_rook];
    }
}

void ZobristKey::reset() {
    hash = 0;
}

void ZobristKey::init() {
    std::mt19937 gen(0);
    std::uniform_int_distribution<std::uint64_t> dist(
        10, std::numeric_limits<std::uint64_t>::max());

    color_key = dist(gen);

    for (int i = 0; i < 8; i++)
        enpassant_keys[i] = dist(gen);

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            castle_keys[j]   = dist(gen);
            piece_keys[i][j] = dist(gen);
            piece_keys[i][j] = dist(gen);
        }
    }
}

void ZobristKey::hash_pieces(Position const &position) {
    for (auto sq = SQ_A1; sq <= SQ_H8; sq++) {
        Piece piece = position.get_piece(sq);
        if (piece != PCE_NULL) {
            hash_piece(sq, piece);
        }
    }
}

void ZobristKey::hash_ep(const Square sq) {
    hash ^= enpassant_keys[compute_file(sq)];
}

void ZobristKey::generate(Position const &position) {
    reset();

    hash_pieces(position);
    hash_castle(0, position.get_castle_rooks());

    if (position.get_side() == CLR_WHITE) {
        hash_side();
    }

    if (position.get_ep() != SQ_NULL) {
        hash_ep(position.get_ep());
    }
}

std::ostream &operator<<(std::ostream &o, const ZobristKey key) {
    return o << std::hex << "0x" << key.hash << std::dec;
}