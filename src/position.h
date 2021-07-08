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
#include "piece_manager.h"
#include "castle_rights.h"
#include "position_history.h"
#include "zobrist.h"

class Position
{
    friend class PositionHistory;

public:
    Position();
    bool set_fen(std::string_view);

    uint64_t friend_bb() const;

    uint64_t enemy_bb() const;

    uint64_t total_bb() const;

    void apply_move(Move);

    void apply_move(Move, int &ply);

    void revert_move();

    void revert_move(int &ply);

    void apply_null_move(int &ply);

    void revert_null_move(int &ply);

    bool move_was_legal() const;

    bool move_is_legal(Move);

    bool move_is_pseudolegal(Move);

    bool move_exists(Move);

    bool apply_move(std::string const &);

    void generate_moves(Movelist&);

    void generate_noisy_moves(Movelist&);

    void generate_quiet_moves(Movelist&);

    bool should_do_null() const;

    void perft(int, uint64_t &, bool = true);

    bool king_in_check() const;

    bool is_drawn() const;

    friend std::ostream &operator<<(std::ostream &, Position const &);

public:
    PieceManager pieces;

    CastleRights castle_rights;

    ZobristKey key;

    PositionHistory history;

private:
    void reset();

    bool parse_fen_side(std::string_view);

    bool parse_fen_hmn(std::string_view);

    void reset_halfmoves();

    void reset_ep();

    bool parse_fen_ep(std::string_view);

    inline void switch_players()
    {
        side = !side;
    }

    Piece apply_normal_move(Move);

    Piece apply_enpassant(Move);

    Piece apply_castle(Move);

    Piece apply_promotion(Move);

    void revert_normal_move(Move, Piece);

    void revert_enpassant(Move, Piece);

    void revert_castle(Move);

    void revert_promotion(Move, Piece);

    void update_ep(Square to);

    PositionHistory::Undo &save(Move = NullMove);

    void restore();

    void restore(Move &, Piece &);

public:
    Color side;
    int half_moves;
    Square ep_sq;
};