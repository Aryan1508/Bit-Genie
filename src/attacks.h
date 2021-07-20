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
#pragma once
#include "bitboard.h"
#include "position.h"
#include "magicmoves.hpp"

namespace Attacks
{
    inline void init()
    {
        initmagicmoves();
    }

    inline uint64_t pawn(uint64_t pawns, Color side)
    {
        uint64_t forward = shift(pawns, relative_forward(side));
        return shift<Direction::west>(forward) | shift<Direction::east>(forward);
    }

    inline uint64_t pawn(Square sq, Color side)
    {
        return BitMask::pawn_attacks[side][sq];
    }

    inline uint64_t knight(Square sq)
    {
        return BitMask::knight_attacks[sq];
    }

    inline uint64_t king(Square sq)
    {
        return BitMask::king_attacks[sq];
    }

    inline uint64_t bishop(Square sq, uint64_t occ)
    {
        return Bmagic(sq, occ);
    }

    inline uint64_t rook(Square sq, uint64_t occ)
    {
        return Rmagic(sq, occ);
    }

    inline uint64_t queen(Square sq, uint64_t occ)
    {
        return Qmagic(sq, occ);
    }

    inline bool square_attacked(Position const &position, Square sq, Color enemy, uint64_t occupancy)
    { 
        uint64_t pawns   = position.get_bb(Pawn  , enemy);
        uint64_t knights = position.get_bb(Knight, enemy);
        uint64_t queens  = position.get_bb(Queen , enemy);
        uint64_t kings   = position.get_bb(King  , enemy);
        uint64_t rooks   = position.get_bb(Rook  , enemy) | queens;
        uint64_t bishops = position.get_bb(Bishop, enemy) | queens;

        return (BitMask::pawn_attacks[!enemy][sq] & pawns) 
            || (bishop(sq, occupancy) & bishops) 
            || (rook  (sq, occupancy) & rooks  )
            || (knight(sq           ) & knights) 
            || (king  (sq           ) & kings  );
    }

    inline bool square_attacked(Position const &position, Square sq, Color enemy)
    {
        return square_attacked(position, sq, enemy, position.get_bb());
    }

    inline uint64_t attackers_to_sq(Position const &position, Square sq)
    {
        uint64_t occ     = position.get_bb();
        uint64_t wpawns  = position.get_bb(Piece::wPawn);
        uint64_t bpawns  = position.get_bb(Piece::bPawn);
        uint64_t knights = position.get_bb(Knight);
        uint64_t queens  = position.get_bb(Queen );
        uint64_t kings   = position.get_bb(King  );
        uint64_t rooks   = position.get_bb(Rook  ) | queens;
        uint64_t bishops = position.get_bb(Bishop) | queens;

        uint64_t p_attackers =  (pawn(sq, Color::White) & bpawns)
                             |  (pawn(sq, Color::Black) & wpawns);
        
        return p_attackers | (knight(sq     ) & knights)
                           | (king  (sq     ) & kings  )
                           | (bishop(sq, occ) & bishops)
                           | (rook  (sq, occ) & rooks  );
    }

    inline uint64_t generate(PieceType piece, Square sq, uint64_t occ)
    {
        switch (piece)
        {
            case Knight: return knight(sq);
            case Bishop: return bishop(sq, occ);
            case Rook  : return rook(sq, occ);
            case Queen : return queen(sq, occ);
            default    : return king(sq);
        }
        return 0;
    }
}