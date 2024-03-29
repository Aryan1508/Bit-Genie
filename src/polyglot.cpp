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
#include "bitboard.h"
#include "polyglot.h"
#include "position.h"

#include <fstream>

std::vector<uint64_t> keys_64{
#include "polyglotkeys"
};

static uint64_t get_piece_key(Piece p, Square sq) {
    constexpr int piece_conv[12]{
        1, 3, 5, 7, 9, 11,
        0, 2, 4, 6, 8, 10
    };

    return keys_64[64 * piece_conv[p] + 8 * compute_rank(sq) + compute_file(sq)];
}

static uint64_t hash_pieces(Position const &position) {
    uint64_t hash = 0;
    for (int i = 0; i < 64; i++) {
        Piece piece = position.get_piece(static_cast<Square>(i));
        if (piece != PCE_NULL)
            hash ^= get_piece_key(piece, static_cast<Square>(i));
    }

    return hash;
}

static uint64_t hash_castle(Position const &position) {
    constexpr int offset = 768;
    uint64_t hash        = 0;

    uint64_t rights = position.get_castle_bits();

    if (test_bit(rights, SQ_G1))
        hash ^= keys_64[offset + 0];

    if (test_bit(rights, SQ_C1))
        hash ^= keys_64[offset + 1];

    if (test_bit(rights, SQ_G8))
        hash ^= keys_64[offset + 2];

    if (test_bit(rights, SQ_C8))
        hash ^= keys_64[offset + 3];

    return hash;
}

static uint64_t hash_enpassant(Position const &position) {
    uint64_t hash        = 0;
    constexpr int offset = 772;

    if (position.get_ep() != SQ_NULL)
        hash ^= keys_64[offset + (int)compute_file(position.get_ep())];

    return hash;
}

static uint64_t hash_turn(Position const &position) {
    if (position.get_side() == CLR_WHITE)
        return keys_64[780];

    return 0;
}

static uint64_t make_key(Position const &position) {
    return hash_pieces(position) ^ hash_castle(position) ^ hash_turn(position) ^ hash_enpassant(position);
}

static Move decode_move(Position const &position, uint16_t move) {
    if (!move)
        return MOVE_NULL;

    int to_file   = (move & 0x7);
    int to_row    = (move & 0x38) >> 3;
    int from_file = (move & 0x1C0) >> 6;
    int from_row  = (move & 0xE00) >> 9;
    int promoted  = (move & 0x7000) >> 12;

    Square from = Square(from_row * 8 + from_file);
    Square to   = Square(to_row * 8 + to_file);

    Piece moving = position.get_piece(from);

    if (compute_piece_type(moving) == PT_KING) {
        if (from == SQ_E1 && to == SQ_H1)
            return Move(SQ_E1, SQ_G1, MVEFLAG_CASTLE);

        if (from == SQ_E1 && to == SQ_A1)
            return Move(SQ_E1, SQ_C1, MVEFLAG_CASTLE);

        if (from == SQ_E8 && to == SQ_H8)
            return Move(SQ_E8, SQ_G8, MVEFLAG_CASTLE);

        if (from == SQ_E8 && to == SQ_A8)
            return Move(SQ_E8, SQ_C8, MVEFLAG_CASTLE);
    }

    if (compute_piece_type(moving) == PT_PAWN && compute_rank(from, position.get_side()) == RANK_7)
        return Move(from, to, PieceType(promoted - 1));

    if (compute_piece_type(moving) == PT_PAWN && to == position.get_ep())
        return Move(from, to, MVEFLAG_ENPASSANT);

    return Move(from, to);
}

namespace PolyGlot {
void Book::open(std::string_view path) {
    std::ifstream fil(path.data(), std::ios::binary | std::ios::ate);

    if (!fil) {
        std::cerr << "Couldn't open " << path << std::endl;
        return;
    }

    size_t size  = fil.tellg();
    size_t count = size / sizeof(Entry);

    if (!count) {
        std::cerr << "No entries found in " << path << std::endl;
        return;
    }
    std::cout << count << " entries in " << path << std::endl;

    entries.clear();
    entries.reserve(count);

    fil.seekg(0);
    for (size_t i = 0; i < count; i++) {
        Entry entry;
        fil.read((char *)&entry, sizeof(Entry));
        entry.move   = __builtin_bswap16(entry.move);
        entry.weight = __builtin_bswap16(entry.weight);
        entry.learn  = __builtin_bswap32(entry.learn);
        entries.push_back(entry);
    }
}

Move Book::probe(Position const &position) const {
    uint64_t key  = make_key(position);
    uint16_t best = 0;
    uint16_t move = 0;

    for (auto const &entry : entries) {
        if (key == __builtin_bswap64(entry.key) && entry.weight > best) {
            best = entry.weight;
            move = entry.move;
        }
    }
    return decode_move(position, move);
}
}