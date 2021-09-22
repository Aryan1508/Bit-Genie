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
#include "utils.h"
#include "types.h"

#include <string>

class Move {
private:
    std::uint16_t data;
    std::int16_t score;

public:
    Move() = default;

    explicit constexpr Move(const std::uint16_t val)
        : data(val), score(0) {
    }

    constexpr Move(const Square from, const Square to)
        : data(from | (to << 6)), score(0) {
        assert(is_ok(from) && is_ok(to));
    }

    constexpr Move(const Square from, const Square to, const MoveFlag flag)
        : data(from | (to << 6) | (flag << 12)), score(0) {
        assert(is_ok(from) && is_ok(to) && is_ok(flag));
    }

    constexpr Move(const Square from, const Square to, const PieceType promoted)
        : data(0x3000 | from | (to << 6) | ((promoted - 1) << 14)), score(0) {
        // clang-format off
        assert(is_ok(from) && is_ok(to) && (promoted == PT_KNIGHT || promoted == PT_BISHOP 
                                         || promoted == PT_ROOK   || promoted == PT_QUEEN));
        // clang-format on
    }

    constexpr auto operator==(Move rhs) const {
        return data == rhs.data;
    }

    constexpr auto operator!=(Move rhs) const {
        return data != rhs.data;
    }

    constexpr auto get_from() const {
        return static_cast<Square>(data & 0x3f);
    }

    constexpr auto get_to() const {
        return static_cast<Square>((data >> 6) & 0x3f);
    }

    constexpr auto get_flag() const {
        return static_cast<MoveFlag>((data >> 12) & 0x3);
    }

    constexpr auto get_promoted() const {
        return static_cast<PieceType>(((data >> 14) & 0x3) + 1);
    }

    constexpr auto get_score() const {
        return score;
    }

    constexpr auto get_move_bits() const {
        return data;
    }

    void set_score(const std::int16_t value) {
        score = value;
    }

    std::string to_str() const;
    friend std::ostream &operator<<(std::ostream &o, Move);
};

using Movelist = FixedList<Move, 256>;

constexpr Move MOVE_NULL = Move(SQ_A1, SQ_A1);

class Position;
bool move_is_capture(Position const &, Move);