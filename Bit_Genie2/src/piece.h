#pragma once
#include <stdint.h>

class Piece {
public:
  enum Color : uint8_t;
  enum Type  : uint8_t;

  Piece(); // Empty piece
  Piece(const Type, const Color);

  Color color() const;
  Type  type() const;
public:
  enum Color: uint8_t {
    white = 0, black = 1, 
  };

  enum Type : uint8_t {
    pawn = 0, knight, bishop,
    rook    , queen , king, empty
  };
private:
  uint8_t data;
};

//     Piece structure ( 8 bits)
// 
// Bits          Use
// 
//  0-3    Type of piece(pawn, knight ...)
//   4    Color of piece(white, black)
// 
//    Example 8 bit number (white queen)
//   
//        Color(Black)
//            |
//       0000 1 101
//              \ /
//             Type (Queen)
//      