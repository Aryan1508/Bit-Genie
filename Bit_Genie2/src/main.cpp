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

  for (uint16_t move : gen.movelist)
  {
    position.apply_move(move);
    if (position.move_was_legal()) {
      uint64_t child = perft(position, depth - 1);
      total += child;
    }
    position.revert_move();
  }

  return total;
}

template<typename Callable, typename... Args>
auto benchmark_function(Callable f, Position& pos, int depth) {
  using namespace std::chrono;
  time_point start = high_resolution_clock::now();
  uint64_t ret = f(pos, depth);
  time_point stop = high_resolution_clock::now();
  long long time_taken = duration_cast<milliseconds>(stop - start).count();
  return std::pair<long long, uint64_t>(time_taken, ret);
}

#define MINIMIZE_MAGIC

int main()
{
  Attacks::init();
  ZobristKey::init();
  
  uint64_t nodes = 0;
  uint64_t total = 0;
  Position position;

  //MoveGenerator gen;
  //gen.generate(position);

  //for (auto move : gen.movelist)
  //{
  //  std::cout << GetMoveFrom(move) << GetMoveTo(move) << '\n';
  //}

  for (int i = 0; i < 10; i++)
  {
    auto [time_taken, ret] = benchmark_function(perft, position, 6);

    std::cout << "time: " << time_taken << " ms\n";
    nodes = ret;
    total += time_taken;
  }
  
  std::cout << "\n\navg time: " << total / 10.0f << " ms\n";
  std::cout << "nodes: " << nodes << "\n\n";
}