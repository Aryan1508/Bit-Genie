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
#include "bitmask.h"
#include "position.h"
#include <sstream>

namespace 
{
    void update_castle_rooks(uint64_t& rooks, Move move)
    {
        constexpr auto& mask = BitMask::castle_updates;
        rooks &= mask[move.from()];
        rooks &= mask[move.to()];
    }
}

void Position::revert_move()
{
    history_ply--;

    auto move     = history[history_ply].move;
    auto captured = history[history_ply].captured;
    castle_rooks  = history[history_ply].castle_rooks;
    halfmoves     = history[history_ply].halfmoves;
    key           = history[history_ply].key;
    ep_sq         = history[history_ply].ep_sq;

    side = !side;
    auto  from     = move.from();
    auto  to       = move.to();
    auto  flag     = move.flag();

    move_piece(to, from);


    if (captured != Piece::Empty && flag != Move::Flag::enpassant)
        add_piece(to, captured);

    if (flag == Move::Flag::promotion)
    {
        remove_piece(from);
        add_piece(from, make_piece(PieceType::Pawn, get_side()));
    }

    else if (flag == Move::Flag::enpassant)
        add_piece(static_cast<Square>(to ^ 8), captured);

    else if (flag == Move::Flag::castle)
    {
        if       (to == Square::C1) move_piece_hash(Square::D1, Square::A1);
        else if  (to == Square::G1) move_piece_hash(Square::F1, Square::H1);
        else if  (to == Square::C8) move_piece_hash(Square::D8, Square::A8);
        else if  (to == Square::G8) move_piece_hash(Square::F8, Square::H8);
    }
}

void Position::apply_move(Move move)
{
    history[history_ply].move         = move;
    history[history_ply].castle_rooks = castle_rooks;
    history[history_ply].halfmoves    = halfmoves++;
    history[history_ply].key          = key;
    history[history_ply].ep_sq        = ep_sq;

    Piece& hist_captured = history[history_ply++].captured = Piece::Empty;

    halfmoves++;
    
    if (ep_sq != Square::bad_sq)
    {
        key.hash_ep(ep_sq);
        ep_sq = Square::bad_sq;
    }

    auto  from     = move.from();
    auto  to       = move.to();
    auto  flag     = move.flag();
    auto  moving   = get_piece(from);
    auto  captured = get_piece(to);

    uint64_t old_rooks = castle_rooks;
    update_castle_rooks(castle_rooks, move);
    key.hash_castle(old_rooks, castle_rooks);

    if (captured != Piece::Empty)
    {
        hist_captured = remove_piece_hash(to);
        halfmoves = 0;
    }

    move_piece_hash(from, to);

    if (moving == Piece::wPawn || moving == Piece::bPawn)
    {
        halfmoves = 0;

        if (flag == Move::Flag::promotion)
        {   
            remove_piece_hash(to);
            add_piece_hash(to, make_piece(move.promoted(), get_side()));
        }
        
        else if (flag == Move::Flag::enpassant)
            hist_captured = remove_piece_hash(static_cast<Square>(to ^ 8));

        // Pawn start
        else if ((to ^ from) == 16)
        {
            uint64_t enemy_pawns = get_bb(PieceType::Pawn, !get_side());
            uint64_t ep_slots    = BitMask::pawn_attacks[get_side()][to ^ 8];

            if (enemy_pawns & ep_slots)
            {
                ep_sq = static_cast<Square>(to ^ 8);
                key.hash_ep(ep_sq);
            }
        }
    }

    else if (flag == Move::Flag::castle)
    {
        if       (to == Square::C1) move_piece_hash(Square::A1, Square::D1);
        else if  (to == Square::G1) move_piece_hash(Square::H1, Square::F1);
        else if  (to == Square::C8) move_piece_hash(Square::A8, Square::D8);
        else if  (to == Square::G8) move_piece_hash(Square::H8, Square::F8);
    }

    side = !side;
    key.hash_side();
}

void Position::apply_nullmove()
{
    history[history_ply].move         = NullMove;
    history[history_ply].castle_rooks = castle_rooks;
    history[history_ply].halfmoves    = halfmoves++;
    history[history_ply].key          = key;
    history[history_ply].ep_sq        = ep_sq;

    if (ep_sq != Square::bad_sq)
        key.hash_ep(ep_sq);

    key.hash_side();
    side = !side;
    history_ply++;
}   

void Position::revert_nullmove()
{
    history_ply--;
    castle_rooks  = history[history_ply].castle_rooks;
    halfmoves     = history[history_ply].halfmoves;
    key           = history[history_ply].key;
    ep_sq         = history[history_ply].ep_sq;

    side = !side;
}

bool move_is_capture(Position const &position, Move move)
{
    return position.get_piece(move.to()) != Piece::Empty;
}

std::string Move::str() const 
{
    std::stringstream o;
    o << from() << to();

    if (flag() == Move::Flag::promotion)
        o << promoted();
        
    return o.str();
}

std::ostream& operator<<(std::ostream& o, Move move)
{
    return o << move.str();
}