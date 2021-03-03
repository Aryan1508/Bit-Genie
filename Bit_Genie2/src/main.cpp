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
  Position position2;

  position.set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  position2.set_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");

  ZobristKey h = position2.hash();

  h.hash_side();
  h.hash_piece(Square::E2, Piece(Piece::pawn, Piece::white));
  h.hash_piece(Square::E4, Piece(Piece::pawn, Piece::white));

  std::cout << "First hash: " << position.hash() << std::endl;
  std::cout << "After E4  : " << position2.hash() << std::endl;
  std::cout << "Afer reset: " << h << std::endl;
}
