#include "attacks.h"
#include <iomanip>
#include "move.h"
#include "position.h"
#include "string_parse.h"
#include <vector>
#include <algorithm>

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
  side_to_play = Color::white;
}

void Position::reset_halfmoves()
{
  half_moves = 0;
}

uint64_t Position::friend_bb() const
{
  return pieces.get_occupancy(side_to_play);
}

uint64_t Position::enemy_bb() const
{
  return pieces.get_occupancy(!side_to_play);
}

uint64_t Position::total_occupancy() const
{
  return friend_bb() | enemy_bb();
}

bool Position::parse_fen_side(std::string_view label)
{
  if (label == "w")
  {
    side_to_play = Color::white;
  }
  else if (label == "b")
  {
    side_to_play = Color::black;
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

Color Position::player() const
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
  uint64_t sq_bb = 1ull << to_int(sq);
  pieces.add_piece(sq, sq_bb, piece);
  key.hash_piece(sq, piece);
}

static inline bool should_update_ep(Square from, Square to, PieceType moving)
{
  int f = to_int(from);
  int t = to_int(to);
  return ((f ^ t) == 16) && moving == PieceType::pawn;
}

void Position::update_ep(Square from, Square to)
{
  uint64_t potential = BitMask::pawn_attacks[to_int(side_to_play)][to_int(to) ^ 8];
  uint64_t enemy_pawns = pieces.get_piece_bb(make_piece(PieceType::pawn, !side_to_play));

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

  PieceType from_pce_t = get_piece_type(from_pce);
  Color from_pce_c = get_piece_color(from_pce);

  Piece captured = pieces.squares[to_int(to)];

  if (get_piece_type(from_pce) == PieceType::pawn)
  {
    reset_halfmoves();
    
    if (should_update_ep(from, to, from_pce_t))
      update_ep(from, to);
    
  }

  if (!(captured == Piece::empty))
  {
    reset_halfmoves();

    pieces.bitboards[to_int(get_piece_type(captured))] ^= (1ull << to_int(to));
    pieces.colors[to_int(get_piece_color(captured))] ^= (1ull << to_int(to));
    key.hash_piece(to, captured);
  }

  pieces.bitboards[to_int(from_pce_t)] ^= ((1ull << to_int(from)) | (1ull << to_int(to)));
  pieces.colors[to_int(from_pce_c)] ^= ((1ull << to_int(from)) | (1ull << to_int(to)));
  
  pieces.squares[to_int(to)] = pieces.squares[to_int(from)];
  pieces.squares[to_int(from)] = Piece::empty;

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

  uint64_t moved((1ull << to_int(from)) | (1ull << to_int(to)));

  pieces.bitboards[to_int(get_piece_type(from_pce))] ^= moved;
  pieces.colors[to_int(get_piece_color(from_pce))] ^= moved;

  pieces.bitboards[to_int(get_piece_type(captured))] ^= (1ull << to_int(ep));
  pieces.colors[to_int(get_piece_color(captured))] ^= (1ull << to_int(ep));

  pieces.squares[to_int(to)] = from_pce;
  pieces.squares[to_int(from)] = Piece::empty;
  pieces.squares[to_int(ep)] = Piece::empty;

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

  pieces.bitboards[to_int(get_piece_type(from_pce))] ^= ((1ull << to_int(from)) | (1ull << to_int(to)));
  pieces.colors[to_int(get_piece_color(from_pce))] ^= ((1ull << to_int(from)) | (1ull << to_int(to)));

  if (!(captured == Piece::empty)) {
    pieces.bitboards[to_int(get_piece_type(captured))] ^= (1ull << to_int(to));
    pieces.colors[to_int(get_piece_color(captured))] ^= (1ull << to_int(to));
  }

  pieces.squares[to_int(from)] = pieces.squares[to_int(to)];
  pieces.squares[to_int(to)] = captured;
}

void Position::revert_enpassant(uint16_t move, Piece captured)
{
  Square from = GetMoveFrom(move);
  Square to = GetMoveTo(move);

  Square ep = to_sq(to_int(GetMoveTo(move)) ^ 8);
  uint64_t ep_bb = 1ull << to_int(ep);
  Piece from_pce = pieces.squares[to_int(to)];

  pieces.bitboards[to_int(get_piece_type(from_pce))] ^= ((1ull << to_int(from)) | (1ull << to_int(to)));
  pieces.colors[to_int(get_piece_color(from_pce))] ^= ((1ull << to_int(from)) | (1ull << to_int(to)));

  pieces.bitboards[to_int(get_piece_type(captured))] ^= ep_bb;
  pieces.colors[to_int(get_piece_color(captured))] ^= ep_bb;

  pieces.squares[to_int(ep)] = captured;
  pieces.squares[to_int(from)] = pieces.squares[to_int(to)];
  pieces.squares[to_int(to)] = Piece::empty;
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
  Square our_king = get_lsb(pieces.get_piece_bb(make_piece(PieceType::king, !side_to_play)));
  return !Attacks::square_attacked(*this, our_king, side_to_play);
}

static void print_move(std::ostream& o, uint16_t move)
{
  o << GetMoveFrom(move) << GetMoveTo(move);
}

static void bb_error(Position const& position) 
{
  std::cout << "Error on pos after ";
  if (position.history.total != 0)
    print_move(std::cout, position.history.previous().move);
  else
    std::cout << "reverting";
  std::cout << "\n";
  std::cin.get();
}