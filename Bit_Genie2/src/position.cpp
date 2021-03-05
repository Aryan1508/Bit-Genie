#include "attacks.h"
#include <iomanip>
#include "move.h"
#include "position.h"
#include "string_parse.h"
#include <vector>



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

void Position::add_piece(Square sq, Piece piece)
{
  assert(is_ok(sq));
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

static inline bool should_update_ep(Square from, Square to, Piece::Type moving)
{
  int f = to_int(from);
  int t = to_int(to);
  return ((f ^ t) == 16) && moving == Piece::pawn;
}

void Position::update_ep(Square from, Square to)
{
  Bitboard potential = BitMask::pawn_attacks[side_to_play][to_int(to) ^ 8];
  Bitboard enemy_pawns = pieces.get_piece_bb(Piece(Piece::pawn, switch_color(side_to_play)));

  if (potential & enemy_pawns)
  {
    ep_sq = to_sq(to_int(to) ^ 8);
  }
}

Piece Position::apply_normal_move(uint16_t move)
{
  CastleRights old_castle = castle_rights;
  Square from = GetMoveFrom(move);
  Square to   = GetMoveTo(move);
  Piece from_pce = pieces.squares[to_int(from)];
  Piece captured = pieces.squares[to_int(to)];

  if (from_pce.get_type() == Piece::pawn)
  {
    reset_halfmoves();
    
    if (should_update_ep(from, to, from_pce.get_type()))
      update_ep(from, to);
  }

  if (move_is_capture(*this, move))
  {
    reset_halfmoves();

    pieces.bitboards[captured.get_type()] ^= Bitboard(to);
    pieces.colors[captured.get_color()] ^= Bitboard(to);
    key.hash_piece(to, captured);
  }

  pieces.bitboards[from_pce.get_type()] ^= (Bitboard(from) | Bitboard(to));
  pieces.colors[from_pce.get_color()] ^= (Bitboard(from) | Bitboard(to));
  
  pieces.squares[to_int(to)] = pieces.squares[to_int(from)];
  pieces.squares[to_int(from)].reset();

  castle_rights.update(move);

  key.hash_piece(from, from_pce);
  key.hash_piece(to, from_pce);
  key.hash_castle(old_castle, castle_rights);

  return captured;
}

void Position::apply_move(uint16_t move)
{
  auto& undo = history.current();
  
  undo.castle = castle_rights;
  undo.ep_sq = ep_sq;
  undo.half_moves = half_moves;
  undo.key = key;
  undo.move = move;
 
  history.total++;

  if (ep_sq != Square::bad)
    key.hash_ep(ep_sq);

  reset_ep();
  half_moves++;

  MoveFlag type = GetMoveType(move);

  if (type == MoveFlag::normal)
    undo.captured = apply_normal_move(move);

  else if (type == MoveFlag::enpassant)
    undo.captured = apply_enpassant(move);

  key.hash_side();
  switch_players();
}

Piece Position::apply_enpassant(uint16_t move)
{
  reset_halfmoves();

  Square from = GetMoveFrom(move);
  Square to = GetMoveTo(move);
  Square ep = to_sq(to_int(GetMoveTo(move)) ^ 8);

  Piece from_pce = pieces.squares[to_int(from)];
  Piece captured = pieces.squares[to_int(ep)];

  Bitboard moved(Bitboard(from) | Bitboard(to));

  pieces.bitboards[from_pce.get_type()] ^= moved;
  pieces.colors[from_pce.get_color()] ^= moved;

  pieces.bitboards[captured.get_type()] ^= Bitboard(ep);
  pieces.colors[captured.get_color()] ^= Bitboard(ep);

  pieces.squares[to_int(to)] = from_pce;
  pieces.squares[to_int(from)].reset();
  pieces.squares[to_int(ep)].reset();

  key.hash_piece(from, from_pce);
  key.hash_piece(to  , from_pce);
  key.hash_piece(ep  , captured);

  return captured;
}

void Position::revert_normal_move(uint16_t move, Piece captured)
{
  Square from = GetMoveFrom(move);
  Square to = GetMoveTo(move);

  Piece from_pce = pieces.squares[to_int(to)];

  pieces.bitboards[from_pce.get_type()] ^= (Bitboard(from) | Bitboard(to));
  pieces.colors[from_pce.get_color()] ^= (Bitboard(from) | Bitboard(to));

  if (!captured.is_empty()) {
    pieces.bitboards[captured.get_type()] ^= Bitboard(to);
    pieces.colors[captured.get_color()] ^= Bitboard(to);
  }

  pieces.squares[to_int(from)] = pieces.squares[to_int(to)];
  pieces.squares[to_int(to)] = captured;
}

void Position::revert_enpassant(uint16_t move, Piece captured)
{
  Square from = GetMoveFrom(move);
  Square to = GetMoveTo(move);

  Square ep = to_sq(to_int(GetMoveTo(move)) ^ 8);
  
  Piece from_pce = pieces.squares[to_int(to)];

  pieces.bitboards[from_pce.get_type()] ^= (Bitboard(from) | Bitboard(to));
  pieces.colors[from_pce.get_color()] ^= (Bitboard(from) | Bitboard(to));

  pieces.bitboards[captured.get_type()].set(ep);
  pieces.colors[captured.get_color()].set(ep);

  pieces.squares[to_int(ep)] = captured;
  pieces.squares[to_int(from)] = pieces.squares[to_int(to)];
  pieces.squares[to_int(to)].reset();
}

void Position::revert_move()
{
  auto& undo = history.previous();
  history.total--;

  key           = undo.key;
  ep_sq         = undo.ep_sq;
  half_moves    = undo.half_moves;
  castle_rights = undo.castle;

  MoveFlag type = GetMoveType(undo.move);

  if (type == MoveFlag::normal)
    revert_normal_move(undo.move, undo.captured);

  else if (type == MoveFlag::enpassant)
    revert_enpassant(undo.move, undo.captured);

  switch_players();
}

bool Position::move_was_legal() const
{
  uint16_t move = history.previous().move;
  Piece moving = pieces.squares[to_int(GetMoveTo(move))];
  Bitboard our_king = pieces.get_piece_bb(Piece(Piece::king, switch_color(side_to_play)));

  if (moving.get_type() == Piece::king)
  {
    return !Attacks::square_attacked(*this, our_king.get_lsb(), side_to_play);
  }
  else
  {
    Square sq = our_king.get_lsb();

    Bitboard occupancy = total_occupancy();

    Bitboard bishops = pieces.get_piece_bb(Piece(Piece::bishop, side_to_play));
    Bitboard rooks   = pieces.get_piece_bb(Piece(Piece::rook  , side_to_play));
    Bitboard queens  = pieces.get_piece_bb(Piece(Piece::queen , side_to_play));

    bishops |= queens;
    rooks |= queens;

    return !((Attacks::bishop(sq, occupancy) & bishops) || (Attacks::rook(sq, occupancy) & rooks));
  }
}