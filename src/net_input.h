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
#include "board.h"
#include "fixed_list.h"

#include <vector>

inline uint16_t calculate_input_index(Square sq, Piece piece) {
    return piece * 64 + sq;
}

struct InputUpdate {
    enum : int8_t { Addition = 1,
                    Removal  = -1 };

    uint16_t index;
    int8_t coeff;

    InputUpdate() = default;

    InputUpdate(Square sq, Piece piece, int8_t coeff)
        : index(calculate_input_index(sq, piece)), coeff(coeff) {
    }
};

using NetworkUpdateList = FixedList<InputUpdate, 4>;
using NetworkInput      = std::vector<uint16_t>;