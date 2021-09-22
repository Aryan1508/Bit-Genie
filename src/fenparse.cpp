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
#include "utils.h"
#include "bitboard.h"
#include "position.h"

#include <sstream>

static constexpr auto c_to_pce(const char pce_label) {
    // clang-format off
    const auto ptl   = std::tolower(pce_label);
    const auto pt    = ptl == 'p' ? PT_PAWN
                     : ptl == 'n' ? PT_KNIGHT
                     : ptl == 'b' ? PT_BISHOP
                     : ptl == 'r' ? PT_ROOK
                     : ptl == 'q' ? PT_QUEEN
                     : ptl == 'k' ? PT_KING : throw std::invalid_argument("c_to_pce");
    // clang-format on
    return std::islower(pce_label) ? make_piece(pt, CLR_BLACK)
                                   : make_piece(pt, CLR_WHITE);
}

static constexpr auto s_to_sq(const std::string_view sq_label) {
    const auto sq = static_cast<Square>((sq_label[0] - 97) + ((sq_label[1] - 49) * 8));
    assert(is_ok(sq));
    return sq;
}

std::string Position::get_fen() const {
    std::stringstream s;
    int empty = 0;

    auto print_empty = [&]() {
        if (empty) {
            s << empty;
        }
        empty = 0;
    };

    for (auto sq = SQ_A1; sq <= SQ_H8; sq++) {
        if (sq != SQ_A1 && sq % 8 == 0) {
            print_empty();
            s << '/';
        }

        const auto idx   = flip_square(static_cast<Square>(sq));
        const auto piece = get_piece(idx);

        if (piece != PCE_NULL) {
            print_empty();
            s << piece;
        } else
            empty++;
    }
    print_empty();
    s << ' ' << side << ' ';

    if (!castle_rooks)
        s << '-';
    else {
        if (test_bit(castle_rooks, SQ_G1))
            s << "K";
        if (test_bit(castle_rooks, SQ_C1))
            s << "Q";
        if (test_bit(castle_rooks, SQ_G8))
            s << "k";
        if (test_bit(castle_rooks, SQ_C8))
            s << "q";
    }
    s << ' ' << ep_sq << ' ' << halfmoves;

    return s.str();
}

void Position::set_fen(std::string_view fen) {
    bitboards.fill(0);
    colors.fill(0);
    pieces.fill(PCE_NULL);
    castle_rooks = 0;
    history_ply  = 0;

    auto parts   = split_string(fen);
    auto counter = SQ_A1;

    for (std::string_view s_rank : split_string(parts[0], '/')) {
        for (char p : s_rank) {
            if (std::isdigit(p)) {
                counter += p - '0';
            } else {
                add_piece(
                    flip_square(counter), c_to_pce(p));
                counter++;
            }
        }
    }

    side = parts[1] == "w" ? CLR_WHITE : CLR_BLACK;

    for (auto r : parts[2]) {
        if (r == '-')
            break;

        Square rook = r == 'k'   ? SQ_G8
                      : r == 'q' ? SQ_C8
                      : r == 'K' ? SQ_G1
                                 : SQ_C1;
        set_bit(castle_rooks, rook);
    }

    ep_sq     = parts[3] == "-" ? SQ_NULL : s_to_sq(parts[3]);
    halfmoves = std::stoi(parts[4]);
    hash      = generate_zobrist_hash(*this);

    network.recalculate_hidden_layer(to_net_input());
}