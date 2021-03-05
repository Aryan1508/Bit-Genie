#include "attacks.h"
#include <iomanip>
#include "move.h"
#include "position.h"
#include "string_parse.h"
#include <vector>

static inline bool should_update_ep(Square from, Square to, Piece moving)
{
  return ((to_int(from) ^ to_int(to)) == 16) && moving.get_type() == Piece::pawn;
}

Position::Position() 
{
  set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Position::reset() 
{
  key.reset();
  pieces.reset();
  castle_rights.reset();
  reset_halfmoves();
  side_to_play = Piece::white;
}

void Position::reset_halfmoves()
{
  half_moves = 0;
}

Bitboard Position::friend_bb() const
{
  return pieces.get_occupancy(side_to_play);
}

Bitboard Position::enemy_bb() const
{
  return pieces.get_occupancy(switch_color(side_to_play));
}

Bitboard Position::total_occupancy() const
{
  return friend_bb() | enemy_bb();
}

bool Position::parse_fen_side(std::string_view label)
{
  if (label == "w")
  {
    side_to_play = Piece::white;
  }
  else if (label == "b")
  {
    side_to_play = Piece::black;
  }
  else
  {
    return false;
  }
  return true;
}

bool Position::parse_fen_hmn(std::string_view fen)
{
  reset_halfmoves();
  if (!string_is_number(fen))
  {
    return false;
  }
  half_moves = std::stoi(fen.data());
  return true;
}

bool Position::set_fen(std::string_view fen)
{
  reset();

  // Save a copy of the position, if fen is invalid
  // revert the changes
  Position temp = *this;

  std::vector<std::string> parts = split_string(fen, ' ');

  bool valid = pieces.parse_fen_board(parts[0])  && parse_fen_side(parts[1])
            && castle_rights.parse_fen(parts[2]) && parse_fen_ep(parts[3])
            && parse_fen_hmn(parts[4]);
  
  if (!valid)
  {
    *this = temp;
    return false;
  }

  key.generate(*this);
  return valid;
}

Square Position::get_ep() const
{
  return ep_sq;
}

Piece::Color Position::player() const
{
  return side_to_play;
}

int Position::get_halfmoves() const
{
  return half_moves;
}

void Position::reset_ep()
{
  ep_sq = Square::bad;
}

bool Position::parse_fen_ep(std::string_view sq)
{
  reset_ep();
  if (sq == "-")
  {
    return true;
  }

  if (!is_valid_sq(sq))
  {
    return false;
  }

  ep_sq = to_sq(sq);
  return true;
}

std::ostream& operator<<(std::ostream& o, Position const& position)
{
  o << position.pieces << "\n";
  o << "\nside to play : " << position.side_to_play;
  o << "\ncastle rights: " << position.castle_rights;
  o << "\nen-passant sq: " << position.ep_sq;
  o << "\nhalf-moves   : " << position.half_moves;
  o << "\nzobrist-key  : " << position.key;
  return o;
}

void Position::switch_players()
{
  side_to_play = switch_color(side_to_play);
}

void Position::add_piece(Square sq, Piece piece)
{
  Bitboard sq_bb(sq);
  pieces.add_piece(sq, sq_bb, piece);
  key.hash_piece(sq, piece);
}

Piece Position::clear_sq(Square sq)
{
  Piece removed = pieces.clear_sq(sq);
  key.hash_piece(sq, removed);
  return removed;
}

void Position::update_ep(Square from, Square to)
{
  auto enemy = switch_color(side_to_play);

  Bitboard potential = BitMask::pawn_attacks[enemy][to_int(to) ^ 8];
  Bitboard enemy_pawns = pieces.get_piece_bb(Piece(Piece::pawn, enemy));

  if (potential & enemy_pawns)
  {
    ep_sq = to_sq(to_int(to) ^ 8);
  }
}

void Position::apply_normal_move(Move move)
{
  CastleRights old_castle = castle_rights;
  Square from = move.from();
  Square to   = move.to();

  Piece moving_piece = clear_sq(from);

  if (moving_piece.get_type() == Piece::pawn || move.is_capture(*this))
  {
    reset_halfmoves();
    
    if (move.is_capture(*this))
      clear_sq(to);
  }

  add_piece(to, moving_piece);
  castle_rights.update(move);
  key.hash_castle(old_castle, castle_rights);

  if (should_update_ep(from, to, moving_piece))
    update_ep(from, to);
}

void Position::apply_move(Move move)
{
  history.add(move, *this);

  if (ep_sq != Square::bad)
    key.hash_ep(ep_sq);

  reset_ep();
  half_moves++;

  switch (move.type())
  {
  case Move::normal:
    apply_normal_move(move);
    break;

  default:
    assert(false);
    break;
  }

  key.hash_side();
  switch_players();
}

void Position::revert_normal_move(Move move, Piece captured)
{
  Square from = move.from();
  Square to = move.to();

  Piece moving = pieces.clear_sq(to);
  pieces.add_piece(from, moving);

  if (!captured.is_empty())
    pieces.add_piece(to, captured);
}

void Position::revert_move(Move move)
{
  Piece captured = history.revert(*this);
  revert_normal_move(move, captured);
  switch_players();
}