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
#include "zobrist.h"
#include "position.h"

#include <array>
#include <random>

static std::array<std::array<ZobristKey, SQ_TOTAL>, PCE_TOTAL> PCE_KEYS;
static std::array<ZobristKey, FILE_TOTAL> EP_KEYS;
static std::array<ZobristKey, SQ_TOTAL> CASTLE_KEYS;
static ZobristKey CLR_KEY;

void init_zobrist_keys() {
    std::mt19937 gen(0);
    std::uniform_int_distribution<uint64_t> dist(10, std::numeric_limits<uint64_t>::max());

    CLR_KEY = dist(gen);

    for (auto i = 0; i < 8; i++)
        EP_KEYS[i] = dist(gen);

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 64; j++) {
            CASTLE_KEYS[j] = dist(gen);
            PCE_KEYS[i][j] = dist(gen);
            PCE_KEYS[i][j] = dist(gen);
        }
    }
}

ZobristKey generate_zobrist_hash(Position const &position) {
    ZobristKey hash = 0;

    for (auto sq = SQ_A1; sq < SQ_TOTAL; sq++) {
        const auto pce = position.get_piece(sq);
        if (pce != PCE_NULL) {
            zobrist_hash_piece(hash, pce, sq);
        }
    }

    if (position.get_ep() != SQ_NULL)
        zobrist_hash_ep(hash, position.get_ep());

    if (position.get_side() == CLR_WHITE)
        zobrist_hash_side(hash);

    zobrist_hash_castle(hash, position.get_castle_bits());
    return hash;
}

void zobrist_hash_piece(ZobristKey &hash, const Piece pce, const Square sq) {
    assert(is_ok(sq) && is_ok(pce));
    hash ^= PCE_KEYS[pce][sq];
}

void zobrist_hash_side(ZobristKey &hash) {
    hash ^= CLR_KEY;
}

void zobrist_hash_ep(ZobristKey &hash, const Square sq) {
    assert(is_ok(sq));
    hash ^= EP_KEYS[compute_file(sq)];
}

void zobrist_hash_castle(ZobristKey &hash, std::uint64_t castle_bits) {
    while (castle_bits) {
        const auto sq = pop_lsb(castle_bits);
        hash ^= CASTLE_KEYS[sq];
    }
}