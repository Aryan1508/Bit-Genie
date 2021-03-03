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
  position.set_fen("PPPP1PPP/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq e4 0 1");

  std::cout << position;
}
