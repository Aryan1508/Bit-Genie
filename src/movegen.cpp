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

namespace 
{
    enum class GenType { all, quiet, noisy };

    template<GenType type>
    uint64_t get_targets(Position const& position)
    {
        return type == GenType::all   ? ~position.get_bb(position.get_side())
            :  type == GenType::noisy ?  position.get_bb(!position.get_side())
                                      : ~position.get_bb();
    }

    constexpr uint64_t get_promotion_rank_bb(Color side)
    {
        return side == White ? BitMask::rank8 : BitMask::rank1;
    }

    void verified_add(Movelist& movelist, Position const& position, Move move)
    {
        if (position.is_legal(move))
            movelist.push_back(move);
    }

    void serialize_bitboard(Movelist& movelist, Position const& position, uint64_t bb, Square from)
    {   
        while(bb)
            verified_add(movelist, position, Move(from, pop_lsb(bb)));
    }

    template<Move::Flag flag = Move::Flag::normal> 
    void serialize_bitboard(Movelist& movelist, Position const& position, uint64_t bb, Direction moved)
    {
        while(bb)
        {
            Square to = pop_lsb(bb);

            if (flag == Move::Flag::promotion)
            {
                verified_add(movelist, position, Move(to - moved, to, PieceType::Knight));
                verified_add(movelist, position, Move(to - moved, to, PieceType::Bishop));
                verified_add(movelist, position, Move(to - moved, to, PieceType::Rook));
                verified_add(movelist, position, Move(to - moved, to, PieceType::Queen));

            }
            else 
                verified_add(movelist, position, Move(to - moved, to, flag));
        }
    }
    template<PieceType pt, Color side>
    void generate_simple_moves(Position const& position, Movelist& movelist, uint64_t targets)
    {
        uint64_t pieces = position.get_bb(pt, side);

        while(pieces)
        {
            Square from = pop_lsb(pieces);
            uint64_t moves = Attacks::generate(pt, from, position.get_bb()) & targets;
            serialize_bitboard(movelist, position, moves, from);
        }
    }

    template<Color side>
    void generate_simple_moves(Position const& position, Movelist& movelist, uint64_t targets)
    {
        generate_simple_moves<King  , side>(position, movelist, targets);
        generate_simple_moves<Knight, side>(position, movelist, targets);
        generate_simple_moves<Bishop, side>(position, movelist, targets);
        generate_simple_moves<Rook  , side>(position, movelist, targets);
        generate_simple_moves<Queen , side>(position, movelist, targets);
    }

    template<Color side>
    void generate_pawn_captures(Position const& position, Movelist& movelist, uint64_t forwarded_pawns)
    {
        constexpr uint64_t promo_bb = get_promotion_rank_bb(side);

        uint64_t captures_l = position.get_bb(!position.get_side()) & shift<Direction::west>(forwarded_pawns);
        uint64_t captures_r = position.get_bb(!position.get_side()) & shift<Direction::east>(forwarded_pawns);
        
        serialize_bitboard(movelist, position, captures_l & ~promo_bb, relative_forward(side) + Direction::west);
        serialize_bitboard(movelist, position, captures_r & ~promo_bb, relative_forward(side) + Direction::east);

        serialize_bitboard<Move::Flag::promotion>(movelist, position, captures_l & promo_bb, relative_forward(side) + Direction::west);
        serialize_bitboard<Move::Flag::promotion>(movelist, position, captures_r & promo_bb, relative_forward(side) + Direction::east);
    }

    template<Color side>
    void generate_pawn_pushes(Position const& position, Movelist& movelist, uint64_t forwarded_pawns)
    {
        constexpr uint64_t promo_bb   = get_promotion_rank_bb(side);
        constexpr uint64_t push_2r_bb = side == White ? BitMask::rank4 : BitMask::rank5;

        uint64_t empty = ~position.get_bb();

        uint64_t push_1 = forwarded_pawns & empty;
        uint64_t push_2 = shift<relative_forward(side)>(push_1) & push_2r_bb & empty;

        serialize_bitboard(movelist, position, push_1 & ~promo_bb, relative_forward(side));
        serialize_bitboard(movelist, position, push_2            , relative_forward(side) + relative_forward(side));

        serialize_bitboard<Move::Flag::promotion>(movelist, position, push_1 & promo_bb, relative_forward(side));
    }

    template<Color side>
    void generate_enpassant(Position const& position, Movelist& movelist, uint64_t forwarded_pawns)
    {
        if (position.get_ep() == bad_sq) return;

        uint64_t ep_bb = 1ull << position.get_ep();

        uint64_t ep_l = ep_bb & shift<Direction::west>(forwarded_pawns);
        uint64_t ep_r = ep_bb & shift<Direction::east>(forwarded_pawns);
        
        serialize_bitboard<Move::Flag::enpassant>(movelist, position, ep_l, relative_forward(side) + Direction::west);
        serialize_bitboard<Move::Flag::enpassant>(movelist, position, ep_r, relative_forward(side) + Direction::east);
    }

    void generate_castle(Position const& position, Movelist& movelist, Square from, Square to, uint64_t occ_cond, uint64_t att_cond)
    {
        if (!test_bit(position.get_castle_rooks(), to))
            return;

        if (occ_cond & position.get_bb())
            return;

        while(att_cond)
            if (Attacks::square_attacked(position, pop_lsb(att_cond), !position.get_side()))
                return;

        verified_add(movelist, position, Move(from, to, Move::Flag::castle));
    }

    template<Color side>
    void generate_castle(Position const& position, Movelist& movelist)
    {
        if (side == White)
        {
            generate_castle(position, movelist, E1, C1, 0x0E, 0x08);
            generate_castle(position, movelist, E1, G1, 0x60, 0x20);
        }
        else 
        {
            generate_castle(position, movelist, E8, C8, 0xE00000000000000 , 0x800000000000000);
            generate_castle(position, movelist, E8, G8, 0x6000000000000000, 0x2000000000000000);
        }
    }

    template<GenType type, Color side>
    void    generate_pawn_moves(Position const& position, Movelist& movelist)
    {
        constexpr Direction forward   = relative_forward(side);

        uint64_t pawns           = position.get_bb(Pawn, side);
        uint64_t forwarded_pawns = shift<forward>(pawns);

        if (type == GenType::all)
        {
            generate_pawn_pushes  <side>(position, movelist, forwarded_pawns);
            generate_pawn_captures<side>(position, movelist, forwarded_pawns);
            generate_enpassant    <side>(position, movelist, forwarded_pawns);
        }
        else if (type == GenType::noisy)
        {
            generate_pawn_captures<side>(position, movelist, forwarded_pawns);
            generate_enpassant    <side>(position, movelist, forwarded_pawns);
        }
        else 
            generate_pawn_pushes  <side>(position, movelist, forwarded_pawns);
    }

    template<GenType type, Color side>
    void generate_moves(Position const& position, Movelist& movelist)
    {
        uint64_t targets = get_targets<type>(position);

        generate_simple_moves<side>(position, movelist, targets);
        generate_pawn_moves<type, side>(position, movelist);

        if (type != GenType::noisy && !position.king_in_check())
            generate_castle<side>(position, movelist);
    }

    template<GenType type>
    void generate_moves(Position const& position, Movelist& movelist)
    {
        position.get_side() == Color::White ? generate_moves<type, Color::White>(position, movelist)
                                            : generate_moves<type, Color::Black>(position, movelist);
    }
}

void Position::generate_legal(Movelist& movelist) const
{
    ::generate_moves<GenType::all>(*this, movelist);
}

void Position::generate_noisy(Movelist& movelist) const
{
    ::generate_moves<GenType::noisy>(*this, movelist);
}

void Position::generate_quiet(Movelist& movelist) const
{
    ::generate_moves<GenType::quiet>(*this, movelist);
}