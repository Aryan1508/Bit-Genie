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
#include "position.h"

#include "utils.h"

Position::Position() {
    set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

NetworkInput Position::to_net_input() const {
    NetworkInput input;
    for (auto sq = SQ_A1; sq < SQ_TOTAL; sq++) {
        if (get_piece(sq) != Piece::PCE_NULL) {
            input.push_back(calculate_input_index(sq, get_piece(sq)));
        }
    }
    return input;
}

std::ostream &operator<<(std::ostream &o, Position const &position) {
    return o << position.get_fen();
}

bool Position::king_in_check() const {
    uint64_t king = get_bb(PT_KING, side);
    return square_is_attacked(get_lsb(king), !side);
}

bool Position::drawn() const {
    for (int i = history_ply - 2; i >= 0 && i >= history_ply - halfmoves;
         i -= 2) {
        if (history[i].key == hash)
            return true;
    }

    if (halfmoves >= 100)
        return true;

    auto const &bbs = bitboards;

    if (bbs[PT_PAWN] || bbs[PT_ROOK] || bbs[PT_QUEEN])
        return false;

    if (bbs[PT_KNIGHT]) {
        if (bbs[PT_BISHOP])
            return false;

        return popcount64(bbs[PT_KNIGHT]) <= 2;
    }

    return !is_several(bbs[PT_BISHOP] & get_bb(CLR_WHITE)) &&
           !is_several(bbs[PT_BISHOP] & get_bb(CLR_BLACK));
}

Score Position::static_evaluation() {
    const int eval = static_cast<int>(network.calculate_last_layer());
    return side == CLR_WHITE ? eval : -eval;
}

bool Position::square_is_attacked(const Square sq, const Color enemy) const {
    return square_is_attacked(sq, enemy, get_bb());
}

bool Position::square_is_attacked(const Square sq, const Color enemy, const std::uint64_t occupancy) const {
    const auto pawns   = get_bb(PT_PAWN, enemy);
    const auto knights = get_bb(PT_KNIGHT, enemy);
    const auto queens  = get_bb(PT_QUEEN, enemy);
    const auto kings   = get_bb(PT_KING, enemy);
    const auto rooks   = get_bb(PT_ROOK, enemy) | queens;
    const auto bishops = get_bb(PT_BISHOP, enemy) | queens;

    return (compute_pawn_attack_bb(sq, !enemy) & pawns) ||
           (compute_bishop_attack_bb(sq, occupancy) & bishops) || (compute_rook_attack_bb(sq, occupancy) & rooks) ||
           (compute_knight_attack_bb(sq) & knights) || (compute_king_attack_bb(sq) & kings);
}

std::uint64_t Position::attackers_to_sq(const Square sq) const {
    const auto occ         = get_bb();
    const auto wpawns      = get_bb(PCE_WPAWN);
    const auto bpawns      = get_bb(PCE_BPAWN);
    const auto knights     = get_bb(PT_KNIGHT);
    const auto queens      = get_bb(PT_QUEEN);
    const auto kings       = get_bb(PT_KING);
    const auto rooks       = get_bb(PT_ROOK) | queens;
    const auto bishops     = get_bb(PT_BISHOP) | queens;
    const auto p_attackers = (compute_pawn_attack_bb(sq, CLR_WHITE) & bpawns) | (compute_pawn_attack_bb(sq, CLR_BLACK) & wpawns);

    return p_attackers | (compute_knight_attack_bb(sq) & knights) | (compute_king_attack_bb(sq) & kings) |
           (compute_bishop_attack_bb(sq, occ) & bishops) | (compute_rook_attack_bb(sq, occ) & rooks);
}