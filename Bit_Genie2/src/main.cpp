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
  position.set_fen("3k4/P7/8/8/8/8/8/1K6 w - - 0 1");

  MoveGenerator<MoveGenType::normal> gen;
  gen.generate(position);
 
  for (auto const move : gen.movelist)
  {
    std::cout << move << '\n';
  }

  std::cout << "Total size: " << gen.movelist.size() << std::endl;
}
