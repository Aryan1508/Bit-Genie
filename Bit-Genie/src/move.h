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
#include "misc.h"
#include <string>

#define CreateMove(from, to, type, promoted) (Move(((from)) | ((to) << 6) | ((to_int(type)) << 12) | (((promoted)-1) << 14)))

enum class MoveFlag : uint8_t
{
    normal,
    enpassant,
    castle,
    promotion
};

enum Move : uint32_t
{
    NullMove
};

inline int16_t move_score(Move move)
{
    return static_cast<int16_t>(move >> 16);
}

constexpr Move move_without_score(Move m)
{
    return Move(m & 0xFFFF);
}

inline bool operator<(Move rhs, Move lhs)
{
    return move_score(rhs) < move_score(lhs);
}

inline bool operator>(Move rhs, Move lhs)
{
    return move_score(rhs) > move_score(lhs);
}

constexpr Square move_from(Move move)
{
    return static_cast<Square>(move & 0x3f);
}

constexpr Square move_to(Move move)
{
    return static_cast<Square>((move >> 6) & 0x3f);
}

constexpr MoveFlag move_flag(Move move)
{
    return static_cast<MoveFlag>((move >> 12) & 0x3);
}

constexpr PieceType move_promoted(Move move)
{
    return static_cast<PieceType>(((move >> 14) & 0x3) + 1);
}

inline void set_move_score(Move &move, int16_t score)
{
    move = static_cast<Move>(move | (score << 16));
}

bool move_is_capture(Position const &positio, Move);
std::string print_move(Move);