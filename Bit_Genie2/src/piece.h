#pragma once
#include <iostream>
#include <stdint.h>

class Piece 
{
public:
  enum Color : uint8_t;
  enum Type  : uint8_t;

  Piece(); // Empty piece
  Piece(const Type, const Color);
  
  // Create piece from fen-string piece letter
  Piece(const char);

  Color get_color() const;
  Type  get_type() const;
  bool  is_empty() const;

  // Generate a letter for a piece 
  // white pawn (p), empty(.)
  char letter() const;
public:
  enum Color: uint8_t
  {
    white = 0, black = 1, 
  };

  enum Type : uint8_t
  {
    pawn = 0, knight, bishop,
    rook    , queen , king, empty = 255
  };
private:
  // Functions to get the color
  // and type of a fen-string piece
  static Type  get_type(const char);
  static Color get_color(const char);

private:
  void create(const Type, const Color);

  uint8_t data;
};

std::ostream& operator<<(std::ostream&, const Piece::Color);
std::ostream& operator<<(std::ostream&, const Piece);

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