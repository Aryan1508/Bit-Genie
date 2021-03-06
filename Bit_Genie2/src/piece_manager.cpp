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

uint64_t PieceManager::get_occupancy(Piece::Color color) const
{
  return colors[color];
}

uint64_t PieceManager::get_piece_bb(Piece piece) const
{
  return bitboards[piece.get_type()] & colors[piece.get_color()];
}

void PieceManager::reset()
{
  colors.fill(0);
  bitboards.fill(0);
  squares.fill(Piece());
}

bool PieceManager::add_piece(const Square sq, const char label)
{
  if (!is_valid_piece(label))
  {
    return false;
  }
  add_piece(sq, Piece(std::string(1, label)));
  return true;
}

void PieceManager::add_piece(Square sq, Piece piece)
{
  add_piece(sq, 1ull << to_int(sq), piece);
}

void PieceManager::add_piece(Square sq, uint64_t sq_bb, Piece piece)
{
  squares[to_int(sq)] = piece;
  colors[piece.get_color()] ^= sq_bb;
  bitboards[piece.get_type()] ^= sq_bb;
}

Piece PieceManager::clear_sq(Square sq)
{
  uint64_t sq_bb = 1ull << to_int(sq);
  Piece piece = squares[to_int(sq)];

  squares[to_int(sq)] = Piece();
  colors[piece.get_color()] ^= sq_bb;
  bitboards[piece.get_type()] ^= sq_bb;
  return piece;
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