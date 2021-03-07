#include "piece.h"

std::ostream& operator<<(std::ostream& o, PieceType type)
{
  constexpr char labels[]
  {
    'p', 'n', 'b', 'r', 'q', 'k', '.'
  };
  return o << labels[type];
}

std::ostream& operator<<(std::ostream& o, Color color)
{
  constexpr char labels[]
  {
    'w', 'b', '\0'
  };

  return o << labels[color];
}

std::ostream& operator<<(std::ostream& o, Piece piece)
{
  constexpr char labels[]
  {
    'P', 'N', 'B', 'R', 'Q', 'K', 
    'p', 'n', 'b', 'r', 'q', 'k', '.'
  };
  return o << labels[piece];
}
