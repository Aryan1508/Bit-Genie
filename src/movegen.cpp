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
#include "movelist.h"
#include "position.h"

namespace {
enum class GenType { all,
                     quiet,
                     noisy };

template <GenType type>
std::uint64_t get_targets(Position const &position) {
    return type == GenType::all     ? ~position.get_bb(position.get_side())
           : type == GenType::noisy ? position.get_bb(!position.get_side())
                                    : ~position.get_bb();
}

constexpr std::uint64_t get_promotion_rank_bb(const Color clr) {
    assert(is_ok(clr));
    return clr == CLR_WHITE ? RANK_8_BB : RANK_1_BB;
}

void verified_add(Movelist &movelist, Position const &position, Move move) {
    if (position.is_legal(move))
        movelist.push_back(move);
}

void serialize_bitboard(
    Movelist &movelist, Position const &position, std::uint64_t bb,
    Square from) {
    while (bb)
        verified_add(movelist, position, Move(from, pop_lsb(bb)));
}

template <Move::Flag flag = Move::Flag::normal>
void serialize_bitboard(
    Movelist &movelist, Position const &position, std::uint64_t bb,
    int delta) {
    while (bb) {
        Square to       = pop_lsb(bb);
        const auto from = static_cast<Square>(to - delta);

        if (flag == Move::Flag::promotion) {
            verified_add(movelist, position, Move(from, to, PT_KNIGHT));
            verified_add(movelist, position, Move(from, to, PT_BISHOP));
            verified_add(movelist, position, Move(from, to, PT_ROOK));
            verified_add(movelist, position, Move(from, to, PT_QUEEN));

        } else
            verified_add(movelist, position, Move(from, to, flag));
    }
}
template <PieceType pt, Color side>
void generate_simple_moves(
    Position const &position, Movelist &movelist, std::uint64_t targets) {
    std::uint64_t pieces = position.get_bb(pt, side);

    while (pieces) {
        Square from = pop_lsb(pieces);
        std::uint64_t moves =
            compute_attack_bb(pt, from, position.get_bb()) & targets;
        serialize_bitboard(movelist, position, moves, from);
    }
}

template <Color side>
void generate_simple_moves(
    Position const &position, Movelist &movelist, std::uint64_t targets) {
    generate_simple_moves<PT_KING, side>(position, movelist, targets);
    generate_simple_moves<PT_KNIGHT, side>(position, movelist, targets);
    generate_simple_moves<PT_BISHOP, side>(position, movelist, targets);
    generate_simple_moves<PT_ROOK, side>(position, movelist, targets);
    generate_simple_moves<PT_QUEEN, side>(position, movelist, targets);
}

template <Color side>
void generate_pawn_captures(
    Position const &position, Movelist &movelist,
    std::uint64_t forwarded_pawns) {
    constexpr std::uint64_t promo_bb = get_promotion_rank_bb(side);

    std::uint64_t captures_l = position.get_bb(!position.get_side()) &
                               shift(forwarded_pawns, DIR_WEST);
    std::uint64_t captures_r = position.get_bb(!position.get_side()) &
                               shift(forwarded_pawns, DIR_EAST);

    serialize_bitboard(
        movelist, position, captures_l & ~promo_bb,
        compute_relative_forward(side) + DIR_WEST);
    serialize_bitboard(
        movelist, position, captures_r & ~promo_bb,
        compute_relative_forward(side) + DIR_EAST);

    serialize_bitboard<Move::Flag::promotion>(
        movelist, position, captures_l & promo_bb,
        compute_relative_forward(side) + DIR_WEST);
    serialize_bitboard<Move::Flag::promotion>(
        movelist, position, captures_r & promo_bb,
        compute_relative_forward(side) + DIR_EAST);
}

template <Color side>
void generate_pawn_pushes(
    Position const &position, Movelist &movelist,
    std::uint64_t forwarded_pawns) {
    constexpr std::uint64_t promo_bb = get_promotion_rank_bb(side);
    constexpr std::uint64_t push_2r_bb =
        side == CLR_WHITE ? RANK_4_BB : RANK_5_BB;

    std::uint64_t empty = ~position.get_bb();

    std::uint64_t push_1 = forwarded_pawns & empty;
    std::uint64_t push_2 =
        shift(push_1, compute_relative_forward(side)) & push_2r_bb & empty;

    serialize_bitboard(
        movelist, position, push_1 & ~promo_bb, compute_relative_forward(side));
    serialize_bitboard(
        movelist, position, push_2,
        compute_relative_forward(side) + compute_relative_forward(side));

    serialize_bitboard<Move::Flag::promotion>(
        movelist, position, push_1 & promo_bb, compute_relative_forward(side));
}

template <Color side>
void generate_enpassant(
    Position const &position, Movelist &movelist,
    std::uint64_t forwarded_pawns) {
    if (position.get_ep() == SQ_NULL)
        return;

    std::uint64_t ep_bb = 1ull << position.get_ep();

    std::uint64_t ep_l = ep_bb & shift(forwarded_pawns, DIR_WEST);
    std::uint64_t ep_r = ep_bb & shift(forwarded_pawns, DIR_EAST);

    serialize_bitboard<Move::Flag::enpassant>(
        movelist, position, ep_l, compute_relative_forward(side) + DIR_WEST);
    serialize_bitboard<Move::Flag::enpassant>(
        movelist, position, ep_r, compute_relative_forward(side) + DIR_EAST);
}

void generate_castle(
    Position const &position, Movelist &movelist, Square from, Square to,
    std::uint64_t occ_cond, std::uint64_t att_cond) {
    if (!test_bit(position.get_castle_bits(), to))
        return;

    if (occ_cond & position.get_bb())
        return;

    while (att_cond)
        if (position.square_is_attacked(
                pop_lsb(att_cond), !position.get_side()))
            return;

    verified_add(movelist, position, Move(from, to, Move::Flag::castle));
}

template <Color side>
void generate_castle(Position const &position, Movelist &movelist) {
    if (side == CLR_WHITE) {
        generate_castle(position, movelist, SQ_E1, SQ_C1, 0x0E, 0x08);
        generate_castle(position, movelist, SQ_E1, SQ_G1, 0x60, 0x20);
    } else {
        generate_castle(
            position, movelist, SQ_E8, SQ_C8, 0xE00000000000000,
            0x800000000000000);
        generate_castle(
            position, movelist, SQ_E8, SQ_G8, 0x6000000000000000,
            0x2000000000000000);
    }
}

template <GenType type, Color side>
void generate_pawn_moves(Position const &position, Movelist &movelist) {
    constexpr Direction forward = compute_relative_forward(side);

    std::uint64_t pawns           = position.get_bb(PT_PAWN, side);
    std::uint64_t forwarded_pawns = shift(pawns, forward);

    if (type == GenType::all) {
        generate_pawn_pushes<side>(position, movelist, forwarded_pawns);
        generate_pawn_captures<side>(position, movelist, forwarded_pawns);
        generate_enpassant<side>(position, movelist, forwarded_pawns);
    } else if (type == GenType::noisy) {
        generate_pawn_captures<side>(position, movelist, forwarded_pawns);
        generate_enpassant<side>(position, movelist, forwarded_pawns);
    } else
        generate_pawn_pushes<side>(position, movelist, forwarded_pawns);
}

template <GenType type, Color side>
void generate_moves(Position const &position, Movelist &movelist) {
    std::uint64_t targets = get_targets<type>(position);

    generate_simple_moves<side>(position, movelist, targets);
    generate_pawn_moves<type, side>(position, movelist);

    if (type != GenType::noisy && !position.king_in_check())
        generate_castle<side>(position, movelist);
}

template <GenType type>
void generate_moves(Position const &position, Movelist &movelist) {
    position.get_side() == CLR_WHITE
        ? generate_moves<type, CLR_WHITE>(position, movelist)
        : generate_moves<type, CLR_BLACK>(position, movelist);
}
} // namespace

void Position::generate_legal(Movelist &movelist) const {
    ::generate_moves<GenType::all>(*this, movelist);
}

void Position::generate_noisy(Movelist &movelist) const {
    ::generate_moves<GenType::noisy>(*this, movelist);
}

void Position::generate_quiet(Movelist &movelist) const {
    ::generate_moves<GenType::quiet>(*this, movelist);
}