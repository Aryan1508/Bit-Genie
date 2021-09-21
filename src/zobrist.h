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
#include <cstdint>

class Position;
class ZobristKey {
public:
    ZobristKey();
    void generate(Position const &);

    void hash_side();
    void hash_ep(Square);
    void hash_piece(Square, Piece);
    void hash_castle(std::uint64_t old, std::uint64_t updated);
    void reset();

    std::uint64_t data() const {
        return hash;
    }

    static void init();

    bool operator==(ZobristKey other) const {
        return hash == other.hash;
    }

    friend std::ostream &operator<<(std::ostream &o, const ZobristKey);

private:
    void hash_pieces(Position const &);

private:
    std::uint64_t hash;
};