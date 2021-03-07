#include "attacks.h"
#include <iomanip>
#include "move.h"
#include "position.h"
#include "string_parse.h"
#include "movegen.h"
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
  side = White;
}

void Position::reset_halfmoves()
{
  half_moves = 0;
}

uint64_t Position::friend_bb() const
{
  return pieces.get_occupancy(side);
}

uint64_t Position::enemy_bb() const
{
  return pieces.get_occupancy(!side);
}

uint64_t Position::total_occupancy() const
{
  return friend_bb() | enemy_bb();
}

bool Position::parse_fen_side(std::string_view label)
{
  if (label == "w")
    side = White;

  else if (label == "b")
    side = Black;

  else
    return false;

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

void Position::reset_ep()
{
  ep_sq = Square::bad_sq;
}

bool Position::parse_fen_ep(std::string_view sq)
{
  reset_ep();
  if (sq == "-")
    return true;

  if (!is_valid_sq(sq))
    return false;

  ep_sq = to_sq(sq);
  return true;
}

std::ostream& operator<<(std::ostream& o, Position const& position)
{
  o << position.pieces << "\n";
  o << "\nside to play : " << position.side;
  o << "\ncastle rights: " << position.castle_rights;
  o << "\nen-passant sq: " << position.ep_sq;
  o << "\nhalf-moves   : " << position.half_moves;
  o << "\nzobrist-key  : " << position.key;
  return o;
}

static inline bool is_double_push(Square from, Square to, PieceType moving)
{
  return (from ^ to) == 16;
}

void Position::update_ep(Square from, Square to)
{
  uint64_t potential = BitMask::pawn_attacks[side][to ^ 8];
  uint64_t enemy_pawns = pieces.get_piece_bb<Pawn>(!side);

  if (potential & enemy_pawns)
  {
    ep_sq = to_sq(to ^ 8);
  }
}

Piece Position::apply_enpassant(uint16_t move)
{
  reset_halfmoves();

  Square from = move_from(move);
  Square to = move_to(move);
  Square ep = to_sq(to_int(move_to(move)) ^ 8);

  Piece from_pce = pieces.squares[from];
  Piece captured = pieces.squares[ep];

  pieces.bitboards[type_of(from_pce)] ^= (1ull << from) | (1ull << to);
  pieces.colors[color_of(from_pce)] ^= (1ull << from) | (1ull << to);

  pieces.bitboards[type_of(captured)] ^= (1ull << ep);
  pieces.colors[color_of(captured)] ^= (1ull << ep);

  pieces.squares[to] = from_pce;
  pieces.squares[from] = Empty;
  pieces.squares[ep] = Empty;

  key.hash_piece(from, from_pce);
  key.hash_piece(to  , from_pce);
  key.hash_piece(ep  , captured);

  return captured;
}

void Position::revert_normal_move(uint16_t move, Piece captured)
{
  Square from = move_from(move);
  Square to = move_to(move);

  Piece from_pce = pieces.squares[to];

  pieces.bitboards[type_of(from_pce)] ^= ((1ull << from) | (1ull << to));
  pieces.colors[color_of(from_pce)] ^= ((1ull << from) | (1ull << to));

  if (!(captured == Empty)) {
    pieces.bitboards[type_of(captured)] ^= (1ull << to);
    pieces.colors[color_of(captured)] ^= (1ull << to);
  }

  pieces.squares[from] = pieces.squares[to];
  pieces.squares[to] = captured;
}

void Position::revert_enpassant(uint16_t move, Piece captured)
{
  Square from = move_from(move);
  Square to = move_to(move);

  Square ep = to_sq(to_int(move_to(move)) ^ 8);
  uint64_t ep_bb = 1ull << to_int(ep);
  Piece from_pce = pieces.squares[to];

  pieces.bitboards[type_of(from_pce)] ^= ((1ull << from) | (1ull << to));
  pieces.colors[color_of(from_pce)] ^= ((1ull << from) | (1ull << to));

  pieces.bitboards[type_of(captured)] ^= ep_bb;
  pieces.colors[color_of(captured)]   ^= ep_bb;

  pieces.squares[ep] = captured;
  pieces.squares[from] = pieces.squares[to];
  pieces.squares[to] = Empty;
}

bool Position::move_was_legal() const
{
  Square our_king = get_lsb(pieces.get_piece_bb(make_piece(King, !side)));
  return !Attacks::square_attacked(*this, our_king, side);
}

static inline uint64_t legal_move_count(Position& position)
{
  MoveGenerator<true> gen;
  gen.generate(position);

  return gen.movelist.size();
}

uint64_t Position::perft(int depth, bool root)
{
  uint64_t nodes = 0;

  if (depth == 1)
  {
    return legal_move_count(*this);
  }
  
  MoveGenerator<true> gen;
  gen.generate(*this);

  for (auto m : gen.movelist)
  {
    apply_move(m);
    auto child = perft(depth - 1, false);
    revert_move();

    if (root)
      std::cout << print_move(m) << ": " << child << '\n';
    nodes += child;
  }

  return nodes;
}

bool Position::move_is_legal(uint16_t move)
{
  Color us = side;
  Square from = move_from(move);
  Square to = move_to(move);

  uint64_t occupancy = total_occupancy();


  if (move_flag(move) == MoveFlag::normal)
  {
    if (type_of(pieces.squares[from]) == King)
    {
      occupancy ^= (1ull << from);
      return !Attacks::square_attacked(*this, to, !side, occupancy);
    }

    else
    {
      
      occupancy ^= (1ull << from) ^ (1ull << to);

      Piece captured = pieces.squares[to];
      uint64_t old = pieces.bitboards[to_int(type_of(captured))];

      if (captured != Empty)
      {
        occupancy ^= (1ull << to);
        pieces.bitboards[to_int(type_of(captured))] ^= (1ull << to);
      }

      uint64_t king = pieces.bitboards[King] & pieces.colors[to_int(side)];
      bool is_legal = !Attacks::square_attacked(*this, get_lsb(king), !side, occupancy);
     
      pieces.bitboards[to_int(type_of(captured))] = old;
      return is_legal;
    }
  }

  else if (move_flag(move) == MoveFlag::enpassant)
  {
    uint64_t occupancy = total_occupancy();
    Square ep = Square(to ^ 8);

    occupancy ^= (1ull << from) ^ (1ull << to) ^ (1ull << (to_int(ep)));
    Piece captured = pieces.squares[to];
    uint64_t old = pieces.bitboards[to_int(type_of(captured))];

    pieces.bitboards[to_int(type_of(captured))] ^= (1ull << (to_int(ep)));
    uint64_t king = pieces.bitboards[to_int(King)] & pieces.colors[to_int(side)];
    bool is_legal = !Attacks::square_attacked(*this, get_lsb(king), !side, occupancy);


    pieces.bitboards[to_int(type_of(captured))] = old;
    return is_legal;
  }

  else if (move_flag(move) == MoveFlag::castle)
  {
    return !Attacks::square_attacked(*this, to, !side, occupancy);
  }
  else
  {
    occupancy ^= (1ull << from) ^ (1ull << to);

    Piece captured = pieces.squares[to];
    uint64_t old = pieces.bitboards[type_of(captured)];

    if (captured != Empty)
    {
      occupancy ^= (1ull << to);
      pieces.bitboards[type_of(captured)] ^= (1ull << to);
    }

    uint64_t king = pieces.bitboards[King] & pieces.colors[side];
    bool is_legal = !Attacks::square_attacked(*this, get_lsb(king), !side, occupancy);

    pieces.bitboards[type_of(captured)] = old;
    return is_legal;
  }

  return true;
}

Piece Position::apply_castle(uint16_t move)
{
  auto old_castle = castle_rights;
  Square from = move_from(move);
  Square to = move_to(move);
  Square rook_from = bad_sq, rook_to = bad_sq;
  Color col;

  switch (to)
  {
  case C1:
    rook_from = A1;
    rook_to = D1;
    col = White;
    break;

  case G1:
    rook_from = H1;
    rook_to = F1;
    col = White;
    break;

  case C8:
    rook_from = A8;
    rook_to = D8;
    col = Black;
    break;

  case G8:
    rook_from = H8;
    rook_to = F8;
    col = Black;
    break;

  default:
    assert(false);
    break;
  }

  PieceType rook = type_of(pieces.squares[rook_from]);
  PieceType king = type_of(pieces.squares[from]);
  
  pieces.bitboards[king] ^= (1ull << from) ^ (1ull << to);
  pieces.bitboards[rook] ^= (1ull << rook_from) ^ (1ull << rook_to);
  pieces.colors[col] ^= (1ull << from) ^ (1ull << to) ^ (1ull << rook_from) ^ (1ull << rook_to);

  pieces.squares[to] = pieces.squares[from];
  pieces.squares[from] = Empty;

  pieces.squares[rook_to] = pieces.squares[rook_from];
  pieces.squares[rook_from] = Empty;

  castle_rights.update(move);

  key.hash_piece(from, make_piece(king, col));
  key.hash_piece(to, make_piece(king, col));
  key.hash_piece(rook_from, make_piece(rook, col));
  key.hash_piece(rook_to, make_piece(rook, col));
  key.hash_castle(old_castle, castle_rights);

  return Empty;
}

void Position::revert_castle(uint16_t move)
{
  Square from = move_from(move);
  Square to = move_to(move);
  Square rook_from = bad_sq, rook_to = bad_sq;
  Color col;

  switch (to)
  {
  case C1:
    rook_from = A1;
    rook_to = D1;
    col = White;
    break;

  case G1:
    rook_from = H1;
    rook_to = F1;
    col = White;
    break;

  case C8:
    rook_from = A8;
    rook_to = D8;
    col = Black;
    break;

  case G8:
    rook_from = H8;
    rook_to = F8;
    col = Black;
    break;

  default:
    assert(false);
    break;
  }

  PieceType rook = type_of(pieces.squares[rook_to]);
  PieceType king = type_of(pieces.squares[to]);

  pieces.bitboards[king] ^= (1ull << from) ^ (1ull << to);
  pieces.bitboards[rook] ^= (1ull << rook_from) ^ (1ull << rook_to);
  pieces.colors[col] ^= (1ull << from) ^ (1ull << to) ^ (1ull << rook_from) ^ (1ull << rook_to);

  pieces.squares[from] = pieces.squares[to];
  pieces.squares[to] = Empty;

  pieces.squares[rook_from] = pieces.squares[rook_to];
  pieces.squares[rook_to] = Empty;
}

Piece Position::apply_normal_move(uint16_t move)
{
  CastleRights old_castle = castle_rights;
  Square from = move_from(move);
  Square to   = move_to(move);
  Piece from_pce = pieces.squares[from];

  PieceType from_pce_t = type_of(from_pce);
  Color from_pce_c = color_of(from_pce);

  Piece captured = pieces.squares[to];

  if (type_of(from_pce) == Pawn)
  {
    reset_halfmoves();
    
    if (is_double_push(from, to, from_pce_t))
      update_ep(from, to);
  }

  if (captured != Empty)
  {
    reset_halfmoves();

    pieces.bitboards[type_of(captured)] ^= (1ull << to);
    pieces.colors[color_of(captured)] ^= (1ull << to);
    key.hash_piece(to, captured);
  }

  pieces.bitboards[from_pce_t] ^= ((1ull << from) | (1ull << to));
  pieces.colors[from_pce_c] ^= ((1ull << from) | (1ull << to));
  
  pieces.squares[to] = pieces.squares[from];
  pieces.squares[from] = Empty;

  castle_rights.update(move);

  key.hash_piece(from, from_pce);
  key.hash_piece(to, from_pce);
  key.hash_castle(old_castle, castle_rights);

  return captured;
}

Piece Position::apply_promotion(uint16_t move)
{
  reset_halfmoves();

  CastleRights old_castle = castle_rights;
  Square from = move_from(move);
  Square to = move_to(move);
  Piece from_pce = pieces.squares[from];

  PieceType from_pce_t = type_of(from_pce);
  Color from_pce_c = color_of(from_pce);

  PieceType prom_pce = move_promoted(move);

  Piece captured = pieces.squares[to];

  if (captured != Empty)
  {
    pieces.bitboards[type_of(captured)] ^= (1ull << to);
    pieces.colors[color_of(captured)] ^= (1ull << to);
    key.hash_piece(to, captured);
  }

  pieces.bitboards[from_pce_t] ^= (1ull << from);
  pieces.bitboards[prom_pce] ^= (1ull << to);
  pieces.colors[from_pce_c] ^= ((1ull << from) | (1ull << to));

  pieces.squares[from] = Empty;
  pieces.squares[to] = make_piece(prom_pce, from_pce_c);

  castle_rights.update(move);

  key.hash_piece(from, from_pce);
  key.hash_piece(to, make_piece(prom_pce, from_pce_c));
  key.hash_castle(old_castle, castle_rights);
  return captured;
}

void Position::revert_promotion(uint16_t move, Piece captured)
{
  Square from = move_from(move);
  Square to = move_to(move);

  Piece original = make_piece(PieceType::Pawn, !side);
  Piece prom_pce = pieces.squares[to];

  pieces.bitboards[type_of(original)] ^= (1ull << from);
  pieces.colors[color_of(original)] ^= (1ull << from) ^ (1ull << to);
  
  pieces.bitboards[type_of(prom_pce)] ^= (1ull << to);

  if (captured != Empty) {
    pieces.bitboards[type_of(captured)] ^= (1ull << to);
    pieces.colors[color_of(captured)] ^= (1ull << to);
  }

  pieces.squares[from] = original;
  pieces.squares[to] = captured;
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

  if (ep_sq != Square::bad_sq)
    key.hash_ep(ep_sq);

  reset_ep();
  half_moves++;

  MoveFlag type = move_flag(move);

  if (type == MoveFlag::normal)
    undo.captured = apply_normal_move(move);

  else if (type == MoveFlag::enpassant)
    undo.captured = apply_enpassant(move);

  else if (type == MoveFlag::castle)
    undo.captured = apply_castle(move);
  else
    undo.captured = apply_promotion(move);

  key.hash_side();
  switch_players();
}

void Position::revert_move()
{
  auto& undo = history.previous();
  history.total--;

  key           = undo.key;
  ep_sq         = undo.ep_sq;
  half_moves    = undo.half_moves;
  castle_rights = undo.castle;

  MoveFlag type = move_flag(undo.move);

  if (type == MoveFlag::normal)
    revert_normal_move(undo.move, undo.captured);

  else if (type == MoveFlag::enpassant)
    revert_enpassant(undo.move, undo.captured);

  else if (type == MoveFlag::castle)
    revert_castle(undo.move);

  else 
    revert_promotion(undo.move, undo.captured);

  switch_players();
}