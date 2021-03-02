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

  PieceManager pieces;
  pieces.set_pieces("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R");

  std::cout << pieces;
}
