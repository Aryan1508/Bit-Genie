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

enum class MoveFlag : uint8_t
{
    normal,
    enpassant,
    castle,
    promotion
};

struct Move 
{
    uint16_t data;
    int16_t score;

    bool operator<(Move rhs) const noexcept 
    {
        return score < rhs.score;
    }

    bool operator<=(Move rhs) const noexcept 
    {
        return score <= rhs.score;
    }

    bool operator>(Move rhs) const noexcept 
    {
        return score > rhs.score;
    }

    bool operator>=(Move rhs) const noexcept 
    {
        return score >= rhs.score;
    }

    bool operator==(Move rhs) const noexcept 
    {
        return data == rhs.data;
    }

    bool operator!=(Move rhs) const noexcept 
    {
        return data != rhs.data;
    }

    Square from() const noexcept 
    {
        return static_cast<Square>(data & 0x3f);   
    }

    Square to() const noexcept 
    {
        return static_cast<Square>((data >> 6) & 0x3f);
    }

    MoveFlag flag() const noexcept 
    {
        return static_cast<MoveFlag>((data >> 12) & 0x3);
    }
    
    PieceType promoted() const noexcept
    {
        return static_cast<PieceType>(((data >> 14) & 0x3) + 1);
    }
    
    friend std::ostream& operator<<(std::ostream& o, Move);
};

constexpr Move NullMove = Move{ 0,  0};

constexpr Move CreateMove(Square from, Square to, MoveFlag type, uint8_t promoted)
{
    return { uint16_t(from | (to << 6) | (to_int(type) << 12) | ((promoted - 1) << 14)), 0 };
}

bool move_is_capture(Position const &, Move);