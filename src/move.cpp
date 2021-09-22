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
#include "move.h"

#include "position.h"
#include <sstream>

namespace {
constexpr uint64_t castle_updates[64]{
    0x004400000000000040, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000000, 0x004400000000000044, 0x004400000000000044, 0x004400000000000004,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004400000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x004000000000000044, 0x004400000000000044, 0x004400000000000044, 0x004400000000000044,
    0x000000000000000044, 0x004400000000000044, 0x004400000000000044, 0x000400000000000044
};

void update_castle_rooks(std::uint64_t &rooks, Move move) {
    constexpr auto &mask = castle_updates;
    rooks &= mask[move.get_from()];
    rooks &= mask[move.get_to()];
}
} // namespace

void Position::revert_move() {
    history_ply--;
    network.revert_hidden_updates();

    auto move     = history[history_ply].move;
    auto captured = history[history_ply].captured;
    castle_rooks  = history[history_ply].castle_rooks;
    halfmoves     = history[history_ply].halfmoves;
    hash          = history[history_ply].key;
    ep_sq         = history[history_ply].ep_sq;

    side      = !side;
    auto from = move.get_from();
    auto to   = move.get_to();
    auto flag = move.get_flag();

    if (flag == MVEFLAG_NORMAL) {
        move_piece(to, from);
        if (captured != PCE_NULL)
            add_piece(to, captured);
    }

    else if (flag == MVEFLAG_PROMOTION) {
        add_piece(from, make_piece(PT_PAWN, side));
        remove_piece(to);

        if (captured != PCE_NULL)
            add_piece(to, captured);
    } else if (flag == MVEFLAG_CASTLE) {
        move_piece(to, from);
        if (to == SQ_C1)
            move_piece(SQ_D1, SQ_A1);
        else if (to == SQ_G1)
            move_piece(SQ_F1, SQ_H1);
        else if (to == SQ_C8)
            move_piece(SQ_D8, SQ_A8);
        else if (to == SQ_G8)
            move_piece(SQ_F8, SQ_H8);
    } else {
        move_piece(to, from);
        add_piece(static_cast<Square>(to ^ 8), captured);
    }
}

void Position::apply_move(Move move) {
    history[history_ply].move         = move;
    history[history_ply].castle_rooks = castle_rooks;
    history[history_ply].halfmoves    = halfmoves++;
    history[history_ply].key          = hash;
    history[history_ply].ep_sq        = ep_sq;

    Piece &hist_captured = history[history_ply++].captured = PCE_NULL;

    halfmoves++;

    if (ep_sq != SQ_NULL) {
        zobrist_hash_ep(hash, ep_sq);
        ep_sq = SQ_NULL;
    }

    NetworkUpdateList updates;
    auto from     = move.get_from();
    auto to       = move.get_to();
    auto flag     = move.get_flag();
    auto moving   = get_piece(from);
    auto captured = get_piece(to);

    std::uint64_t old_rooks = castle_rooks;
    update_castle_rooks(castle_rooks, move);
    zobrist_hash_castle(hash, old_rooks ^ castle_rooks);

    if (flag == MVEFLAG_NORMAL) {
        if (captured != PCE_NULL) {
            hist_captured = remove_piece_hash(to);
            halfmoves     = 0;
            updates.push_back(
                InputUpdate(to, hist_captured, InputUpdate::Removal));
        }

        updates.push_back(InputUpdate(from, moving, InputUpdate::Removal));
        updates.push_back(InputUpdate(to, moving, InputUpdate::Addition));

        move_piece_hash(from, to);

        if (moving == PCE_WPAWN || moving == PCE_BPAWN) {
            halfmoves = 0;
            if ((to ^ from) == 16) {
                std::uint64_t enemy_pawns = get_bb(PT_PAWN, !get_side());
                std::uint64_t ep_slots =
                    compute_pawn_attack_bb(static_cast<Square>(to ^ 8), get_side());

                if (enemy_pawns & ep_slots) {
                    ep_sq = static_cast<Square>(to ^ 8);
                    zobrist_hash_ep(hash, ep_sq);
                }
            }
        }
    }

    else if (flag == MVEFLAG_PROMOTION) {
        halfmoves = 0;

        if (captured != PCE_NULL) {
            hist_captured = remove_piece_hash(to);
            updates.push_back(
                InputUpdate(to, hist_captured, InputUpdate::Removal));
        }

        Piece promoted = make_piece(move.get_promoted(), get_side());
        remove_piece_hash(from);
        add_piece_hash(to, promoted);

        updates.push_back(InputUpdate(from, moving, InputUpdate::Removal));
        updates.push_back(InputUpdate(to, promoted, InputUpdate::Addition));
    }

    else if (flag == MVEFLAG_CASTLE) {
        Square rook_from = SQ_NULL, rook_to = SQ_NULL;
        if (to == SQ_C1) {
            rook_from = SQ_A1;
            rook_to   = SQ_D1;
        } else if (to == SQ_G1) {
            rook_from = SQ_H1;
            rook_to   = SQ_F1;
        } else if (to == SQ_C8) {
            rook_from = SQ_A8;
            rook_to   = SQ_D8;
        } else if (to == SQ_G8) {
            rook_from = SQ_H8;
            rook_to   = SQ_F8;
        }

        Piece king = moving, rook = get_piece(rook_from);

        move_piece_hash(from, to);
        move_piece_hash(rook_from, rook_to);

        updates.push_back(InputUpdate(from, king, InputUpdate::Removal));
        updates.push_back(InputUpdate(to, king, InputUpdate::Addition));
        updates.push_back(InputUpdate(rook_from, rook, InputUpdate::Removal));
        updates.push_back(InputUpdate(rook_to, rook, InputUpdate::Addition));
    } else {
        halfmoves = 0;
        move_piece_hash(from, to);
        Square cap_sq = static_cast<Square>(to ^ 8);

        hist_captured = remove_piece_hash(cap_sq);

        updates.push_back(InputUpdate(from, moving, InputUpdate::Removal));
        updates.push_back(InputUpdate(to, moving, InputUpdate::Addition));
        updates.push_back(
            InputUpdate(cap_sq, hist_captured, InputUpdate::Removal));
    }

    side = !side;
    zobrist_hash_side(hash);
    network.update_hidden_layer(updates);
}

void Position::apply_nullmove() {
    history[history_ply].key   = hash;
    history[history_ply].ep_sq = ep_sq;
    history[history_ply].move  = MOVE_NULL;

    history_ply++;
    halfmoves++;

    if (ep_sq != SQ_NULL)
        zobrist_hash_ep(hash, ep_sq);

    zobrist_hash_side(hash);
    side = !side;
}

void Position::revert_nullmove() {
    history_ply--;
    halfmoves--;
    hash  = history[history_ply].key;
    ep_sq = history[history_ply].ep_sq;

    side = !side;
}

bool move_is_capture(Position const &position, Move move) {
    return position.get_piece(move.get_to()) != PCE_NULL;
}

std::string Move::to_str() const {
    std::stringstream o;
    o << get_from() << get_to();

    if (get_flag() == MVEFLAG_PROMOTION)
        o << get_promoted();

    return o.str();
}

std::ostream &operator<<(std::ostream &o, Move move) {
    return o << move.to_str();
}