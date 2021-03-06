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

template<typename Callable, typename... Args>
auto benchmark_function(Callable f, Position& pos, int depth) {
  using namespace std::chrono;
  time_point start = high_resolution_clock::now();
  uint64_t ret = f(pos, depth);
  time_point stop = high_resolution_clock::now();
  long long time_taken = duration_cast<milliseconds>(stop - start).count();
  return std::pair<long long, uint64_t>(time_taken, ret);
}

auto foo(Position& position, int d)
{
  auto nodes = position.perft(d);
  return nodes;
}

int main()
{
  Attacks::init();
  ZobristKey::init();
  Position position;

  auto[time_taken, ret] = benchmark_function(foo, position, 6);

  std::cout << "\nnodes: " << ret << '\n';
  std::cout << "time  : " << time_taken << " ms\n";

}