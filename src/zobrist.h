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

class ZobristKey
{
public:
    ZobristKey();
    void generate(Position const &);

    void hash_side();
    void hash_ep(const Square);
    void hash_piece(const Square, const Piece);
    void hash_castle(const CastleRights, const CastleRights);
    void reset();

    uint64_t data() const { return hash; }

    static void init();

    bool operator==(ZobristKey other) const
    {
        return hash == other.hash;
    }

    friend std::ostream &operator<<(std::ostream &o, const ZobristKey);

private:
    void hash_pieces(Position const &);

private:
    uint64_t hash;
};