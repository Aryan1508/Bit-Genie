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


uint64_t perft(Position& position, int depth)
{
  uint64_t total = 0;

  if (depth == 0)
    return 1;

  MoveGenerator gen;
  gen.generate(position);

  for (Move move : gen.movelist)
  {
    position.apply_move(move);
    if (position.move_was_legal(move)) {
      total += perft(position, depth - 1);
    }
    position.revert_move(move);
  }

  return total;
}

template<typename Callable, typename... Args>
auto benchmark_function(Callable f, Position& pos, int depth) {
  using namespace std::chrono;
  time_point start = high_resolution_clock::now();
  auto ret = f(pos, depth);
  time_point stop = high_resolution_clock::now();
  long long time_taken = duration_cast<milliseconds>(stop - start).count();
  return std::tuple<long long, decltype(ret)>{time_taken, ret};
}

int main()
{
  Attacks::init();
  ZobristKey::init();

  MoveGenerator gen;
  Position position;
  
  gen.generate(position);

  auto [time, ret] = benchmark_function(perft, position, 6);

  std::cout << "time taken: " << time << " ms\n.";
}