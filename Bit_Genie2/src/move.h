#pragma once
#include "misc.h"
#include "piece.h"

class Move
{
public:
  enum GenType { normal, enpassant, castle, promotion };
  enum PromPiece { knight, bishop, rook, queen };

  inline Move()
  {
    data = 0;
  }

  Move(Square from, Square to, GenType type = normal, PromPiece promoted = PromPiece::knight)
  {
    data = (to_int(from)) | (to_int(to) << 6) | (type << 12) | (promoted << 14);
  }

  Square from() const;
  Square to()   const;
  GenType type() const;
  Piece::Type promoted() const;

  bool operator==(const Move other) const
  {
    return data == other.data;
  }
  
  bool operator!=(const Move other) const
  {
    return data != other.data;
  }

  bool is_capture(Position const&);
private:
  uint16_t data;
};

std::ostream& operator<<(std::ostream& o, const Move);