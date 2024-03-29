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
#include "attacks.h"
#include "position.h"
#include "movelist.h"
#include "bitboard.h"

namespace {
enum MoveGenType : uint8_t {
    MOVEGEN_ALL,
    MOVEGEN_QUIET,
    MOVEGEN_NOISY
};

template <MoveGenType type>
uint64_t get_targets(Position const &position) {
    return type == MoveGenType::MOVEGEN_ALL     ? ~position.get_bb(position.get_side())
           : type == MoveGenType::MOVEGEN_NOISY ? position.get_bb(!position.get_side())
                                                : ~position.get_bb();
}

constexpr uint64_t get_promotion_rank_bb(Color side) {
    return side == CLR_WHITE ? RANK_8_BB : RANK_1_BB;
}

void verified_add(Movelist &movelist, Position const &position, Move move) {
    if (position.is_legal(move))
        movelist.push_back(move);
}

void serialize_bitboard(Movelist &movelist, Position const &position, uint64_t bb, Square from) {
    while (bb)
        verified_add(movelist, position, Move(from, pop_lsb(bb)));
}

template <MoveFlag flag = MVEFLAG_NORMAL>
void serialize_bitboard(Movelist &movelist, Position const &position, uint64_t bb, int moved) {
    while (bb) {
        auto to   = pop_lsb(bb);
        auto from = static_cast<Square>(to - moved);

        if (flag == MVEFLAG_PROMOTION) {
            verified_add(movelist, position, Move(from, to, PT_KNIGHT));
            verified_add(movelist, position, Move(from, to, PT_BISHOP));
            verified_add(movelist, position, Move(from, to, PT_ROOK));
            verified_add(movelist, position, Move(from, to, PT_QUEEN));

        } else
            verified_add(movelist, position, Move(from, to, flag));
    }
}
template <PieceType pt, Color side>
void generate_simple_moves(Position const &position, Movelist &movelist, uint64_t targets) {
    auto pieces = position.get_bb(pt, side);

    while (pieces) {
        auto from  = pop_lsb(pieces);
        auto moves = generate_attacks_bb(pt, from, position.get_bb()) & targets;
        serialize_bitboard(movelist, position, moves, from);
    }
}

template <Color side>
void generate_simple_moves(Position const &position, Movelist &movelist, uint64_t targets) {
    generate_simple_moves<PT_KING, side>(position, movelist, targets);
    generate_simple_moves<PT_KNIGHT, side>(position, movelist, targets);
    generate_simple_moves<PT_BISHOP, side>(position, movelist, targets);
    generate_simple_moves<PT_ROOK, side>(position, movelist, targets);
    generate_simple_moves<PT_QUEEN, side>(position, movelist, targets);
}

template <Color side>
void generate_pawn_captures(Position const &position, Movelist &movelist, uint64_t forwarded_pawns) {
    constexpr uint64_t promo_bb = get_promotion_rank_bb(side);

    auto captures_l = position.get_bb(!position.get_side()) & shift<DIR_WEST>(forwarded_pawns);
    auto captures_r = position.get_bb(!position.get_side()) & shift<DIR_EAST>(forwarded_pawns);

    serialize_bitboard(movelist, position, captures_l & ~promo_bb, compute_relative_forward(side) + DIR_WEST);
    serialize_bitboard(movelist, position, captures_r & ~promo_bb, compute_relative_forward(side) + DIR_EAST);

    serialize_bitboard<MVEFLAG_PROMOTION>(movelist, position, captures_l & promo_bb, compute_relative_forward(side) + DIR_WEST);
    serialize_bitboard<MVEFLAG_PROMOTION>(movelist, position, captures_r & promo_bb, compute_relative_forward(side) + DIR_EAST);
}

template <Color side>
void generate_pawn_pushes(Position const &position, Movelist &movelist, uint64_t forwarded_pawns) {
    constexpr uint64_t promo_bb   = get_promotion_rank_bb(side);
    constexpr uint64_t push_2r_bb = side == CLR_WHITE ? RANK_4_BB : RANK_5_BB;

    auto empty  = ~position.get_bb();
    auto push_1 = forwarded_pawns & empty;
    auto push_2 = shift<compute_relative_forward(side)>(push_1) & push_2r_bb & empty;

    serialize_bitboard(movelist, position, push_1 & ~promo_bb, compute_relative_forward(side));
    serialize_bitboard(movelist, position, push_2, compute_relative_forward(side) + compute_relative_forward(side));

    serialize_bitboard<MVEFLAG_PROMOTION>(movelist, position, push_1 & promo_bb, compute_relative_forward(side));
}

template <Color side>
void generate_enpassant(Position const &position, Movelist &movelist, uint64_t forwarded_pawns) {
    if (position.get_ep() == SQ_NULL)
        return;

    auto ep_bb = 1ull << position.get_ep();
    auto ep_l  = ep_bb & shift<DIR_WEST>(forwarded_pawns);
    auto ep_r  = ep_bb & shift<DIR_EAST>(forwarded_pawns);

    serialize_bitboard<MVEFLAG_ENPASSANT>(movelist, position, ep_l, compute_relative_forward(side) + DIR_WEST);
    serialize_bitboard<MVEFLAG_ENPASSANT>(movelist, position, ep_r, compute_relative_forward(side) + DIR_EAST);
}

void generate_castle(Position const &position, Movelist &movelist, Square from, Square to, uint64_t occ_cond, uint64_t att_cond) {
    if (!test_bit(position.get_castle_bits(), to))
        return;

    if (occ_cond & position.get_bb())
        return;

    while (att_cond)
        if (square_is_attacked(position, pop_lsb(att_cond), !position.get_side()))
            return;

    verified_add(movelist, position, Move(from, to, MVEFLAG_CASTLE));
}

template <Color side>
void generate_castle(Position const &position, Movelist &movelist) {
    if (side == CLR_WHITE) {
        generate_castle(position, movelist, SQ_E1, SQ_C1, 0x0E, 0x08);
        generate_castle(position, movelist, SQ_E1, SQ_G1, 0x60, 0x20);
    } else {
        generate_castle(position, movelist, SQ_E8, SQ_C8, 0xE00000000000000, 0x800000000000000);
        generate_castle(position, movelist, SQ_E8, SQ_G8, 0x6000000000000000, 0x2000000000000000);
    }
}

template <MoveGenType type, Color side>
void generate_pawn_moves(Position const &position, Movelist &movelist) {
    constexpr auto forward = compute_relative_forward(side);
    auto pawns             = position.get_bb(PT_PAWN, side);
    auto forwarded_pawns   = shift<forward>(pawns);

    if (type == MoveGenType::MOVEGEN_ALL) {
        generate_pawn_pushes<side>(position, movelist, forwarded_pawns);
        generate_pawn_captures<side>(position, movelist, forwarded_pawns);
        generate_enpassant<side>(position, movelist, forwarded_pawns);
    } else if (type == MoveGenType::MOVEGEN_NOISY) {
        generate_pawn_captures<side>(position, movelist, forwarded_pawns);
        generate_enpassant<side>(position, movelist, forwarded_pawns);
    } else
        generate_pawn_pushes<side>(position, movelist, forwarded_pawns);
}

template <MoveGenType type, Color side>
void generate_moves(Position const &position, Movelist &movelist) {
    auto targets = get_targets<type>(position);

    generate_simple_moves<side>(position, movelist, targets);
    generate_pawn_moves<type, side>(position, movelist);

    if (type != MoveGenType::MOVEGEN_NOISY && !position.king_in_check())
        generate_castle<side>(position, movelist);
}

template <MoveGenType type>
void generate_moves(Position const &position, Movelist &movelist) {
    position.get_side() == CLR_WHITE ? generate_moves<type, CLR_WHITE>(position, movelist)
                                     : generate_moves<type, CLR_BLACK>(position, movelist);
}
}

void Position::generate_legal(Movelist &movelist) const {
    ::generate_moves<MoveGenType::MOVEGEN_ALL>(*this, movelist);
}

void Position::generate_noisy(Movelist &movelist) const {
    ::generate_moves<MoveGenType::MOVEGEN_NOISY>(*this, movelist);
}

void Position::generate_quiet(Movelist &movelist) const {
    ::generate_moves<MoveGenType::MOVEGEN_QUIET>(*this, movelist);
}