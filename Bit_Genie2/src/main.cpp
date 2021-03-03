#include "bitboard.h"
#include "io.h"
#include "piece.h"
#include "Square.h"
#include "zobrist.h"
#include "castle_rights.h"
#include "position.h"

int main() 
{
  ZobristKey::init();

  Position position;
 
  std::cout << position;
}
