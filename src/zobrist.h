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
#include "types.h"
#include <cstdint>

class Position;

typedef std::uint64_t ZobristKey;

ZobristKey generate_zobrist_hash(Position const &);

void zobrist_hash_piece(ZobristKey &, const Piece, const Square);

void zobrist_hash_side(ZobristKey &);

void zobrist_hash_ep(ZobristKey &, const Square);

void zobrist_hash_castle(ZobristKey &, const std::uint64_t castle_bits);

void init_zobrist_keys();