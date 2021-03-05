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
  position.set_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");

  MoveGenerator<MoveGenType::normal> gen;
  gen.generate(position);
 
  for (auto const move : gen.movelist)
  {
    std::cout << move << '\n';
  }

  std::cout << "Total size: " << gen.movelist.size() << std::endl;
}
