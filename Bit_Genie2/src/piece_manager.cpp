#include "piece_manager.h"
#include "Square.h"
#include "string_parse.h"

static int to_digit(const char c) {
  return c - '0';
}

static bool contains_whitespaces(std::string_view str)
{
  return str.find(' ') != str.npos;
}

static bool is_valid_piece(const char piece)
{
  std::string valid_pieces = "pnbrqk"; // All piece letters used in fens
  return valid_pieces.find(std::tolower(piece)) != valid_pieces.npos;
}

// This 'increment' refers to the number
// in some fen strings which skips a few squares
// It can only be in between 0 and 9
static bool is_valid_increment(const int increment)
{
  return increment > 0 && increment < 9;
}

PieceManager::PieceManager() 
{
  reset();
}

Bitboard PieceManager::get_occupancy(Piece::Color color) const
{
  return colors[color];
}

Bitboard PieceManager::get_piece_bb(Piece piece) const
{
  return pieces[piece.get_type()] & colors[piece.get_color()];
}

void PieceManager::reset()
{
  colors.fill(0);
  pieces.fill(0);
  squares.fill(Piece());
}

Square PieceManager::get_king_sq(Piece::Color color) const 
{
  Bitboard king = get_piece_bb(Piece(Piece::king, color));
  return king.get_lsb();
}

bool PieceManager::add_piece(const Square sq, const char label)
{
  if (!is_valid_piece(label))
  {
    return false;
  }
  add_piece(sq, Piece(label));
  return true;
}

void PieceManager::add_piece(Square sq, const Piece piece)
{
  squares[to_int(sq)] = piece;
  colors[to_int(piece.get_color())].set(sq);
  pieces[to_int(piece.get_type())].set(sq);
}

bool PieceManager::add_rank(Square& counter, std::string_view rank)
{
  for (auto const c : rank)
  {
    if (!is_ok(counter))
      return false;

    if (std::isalpha(c))
    {
      if (!add_piece(flip_square(counter), c))
      {
        return false;
      }
      counter++;
    }

    else if (std::isdigit(c))
    {
      const int increment = to_digit(c);
      if (!is_valid_increment(increment))
      {
        return false;
      }
      counter += increment;
    }
    else
    {
      return false;
    }
  }
  return true;
}

Piece const& PieceManager::get_piece(const Square sq) const
{
  assert(is_ok(sq));
  return squares[to_int(sq)];
}

bool PieceManager::parse_fen_board(std::string_view fen)
{
  reset();
  // pieces fen cannot contain whitespaces
  if (contains_whitespaces(fen))
  {
    return false;
  }

  // Start from A1 and fill up letter by letter(piece by piece) 
  // until H8. If the fen is valid then the final count must be 64
  // or the total number of squares
  Square square_counter = Square::A1;

  for (auto const& rank : split_string(fen, '/'))
  {
    if (!add_rank(square_counter, rank))
    {
      return false;
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& o, PieceManager const& pm)
{
  for (Square sq = Square::A1; sq <= Square::H8; sq++)
  {
    if (to_int(sq) % 8 == 0)
      o << '\n';

    o << pm.squares[to_int(flip_square(sq))].letter() << ' ';
  }
  return o;
}