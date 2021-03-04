#include "bitboard.h"
#include "castle_rights.h"
#include "io.h"
#include "piece.h"
#include "position.h"
#include "Square.h"
#include "move.h"
#include "movegen.h"
#include "zobrist.h"

int main() 
{
  Attacks::init();
  ZobristKey::init();

  Position position;

  MoveGenerator<MoveGenType::normal> generator;
  generator.generate(position);

  for (auto const move : generator.movelist)
  {
    std::cout << move << '\n';
  }

}
