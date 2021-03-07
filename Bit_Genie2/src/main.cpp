#include "bitboard.h"
#include "castle_rights.h"
#include "io.h"
#include "piece.h"
#include "position.h"
#include "Square.h"
#include "move.h"
#include "movegen.h"
#include "zobrist.h"
#include <chrono>

int main()
{  
  Attacks::init(); 
  ZobristKey::init();
  Position position;

  using namespace std::chrono;
  position.set_fen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0");

  std::cout << position;

  position.apply_move(Move(Square::E1, Square::G1, MoveFlag::castle, 1));
  position.apply_move(Move(Square::E8, Square::G8, MoveFlag::castle, 1));
  position.revert_move();
  position.revert_move();

  std::cout << position;


  for (auto bb : position.pieces.colors)
  {
    print_uint64_t(bb);
    std::cout << '\n'; std::cin.get();
  }

  /*auto start = high_resolution_clock::now();
  uint64_t nodes = position.perft(2);
  auto stop = high_resolution_clock::now();
  long long time_taken = duration_cast<milliseconds>(stop - start).count();

  printf("\nnodes: %llu\n", nodes);
  printf("time : %llu ms\n", time_taken);*/
}