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


uint64_t perft(Position& position, int depth, bool at_root)
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
      uint64_t child = perft(position, depth - 1, false);

      if (at_root)
        std::cout << GetMoveFrom(move) << GetMoveTo(move) <<  (int)GetMoveType(move) << ": " << child << '\n';

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
  uint64_t ret = f(pos, depth, true);
  time_point stop = high_resolution_clock::now();
  long long time_taken = duration_cast<milliseconds>(stop - start).count();
  return std::pair<long long, uint64_t>(time_taken, ret);
}

int main()
{
  Attacks::init();
  ZobristKey::init();

  MoveGenerator gen;
  Position position;

  position.apply_move(Move(Square::E2, Square::E4, MoveFlag::normal, 1));
  position.apply_move(Move(Square::B8, Square::C6, MoveFlag::normal, 1));
  position.apply_move(Move(Square::E4, Square::E5, MoveFlag::normal, 1));
  position.apply_move(Move(Square::A8, Square::B8, MoveFlag::normal, 1));

  auto [time_taken, ret] = benchmark_function(perft, position, 1);

  std::cout << "\nnodes: " << ret << '\n';
  std::cout << "time : " << time_taken << " ms.\n";
}

// 4,865,609
// 37,288,025