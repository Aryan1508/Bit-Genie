#include <iomanip>
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

ZobristKey Position::hash() const 
{
  return key;
}

void Position::reset_halfmoves()
{
  half_moves = 0;
}

Piece const& Position::get_piece(const Square sq) const
{
  return pieces.squares[to_int(sq)];
}

Bitboard Position::get_piece_bb(Piece::Type type) const
{
  return pieces.get_piece_bb(Piece(type, side_to_play));
}

Bitboard Position::enemy_bb() const
{
  return pieces.get_occupancy(switch_color(side_to_play));
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
            && castle_rights.parse_fen(parts[2]) && pieces.parse_fen_ep(parts[3])
            && parse_fen_hmn(parts[4]);
  
  if (!valid)
  {
    *this = temp;
    return false;
  }

  key.generate(*this);
  return valid;
}

CastleRights Position::get_castle_rights() const
{
  return castle_rights;
}

Square Position::get_ep() const
{
  return pieces.ep_sq;
}

Piece::Color Position::player() const
{
  return side_to_play;
}

std::ostream& operator<<(std::ostream& o, Position const& position)
{
  o << position.pieces << "\n";
  o << "\nside to play : " << position.side_to_play;
  o << "\ncastle rights: " << position.castle_rights;
  o << "\nen-passant sq: " << position.pieces.ep_sq;
  o << "\nhalf-moves   : " << position.half_moves;
  o << "\nzobrist-key  : " << position.key;
  return o;
}