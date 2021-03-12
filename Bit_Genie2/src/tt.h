#pragma once
#include "misc.h"
#include "move.h"
#include <vector>

struct TEntry
{
  uint64_t hash = 0;
  Move     move = NullMove;
};

class TTable
{
public:
  TTable();

  TTable(int mb) { resize(mb); }

  void resize(int);
  void add(Position const&, Move);
  void reset()
  {
    std::fill(entries.begin(), entries.end(), TEntry{ 0, NullMove });
  }

  TEntry& retrieve(Position const&);

private:
  std::vector<TEntry> entries;
};

/*
r . b . . r k .
. p p q . p p .
. . n p . n . p
p . b . p . . .
Q . B . P . . .
. . P P . N . P
P P . N . P P .
R . B . . R K .
side to play : w
castle rights:
en-passant sq: -
half-moves   : 2
zobrist-key  : 0xf9b862bb7e3417f0
*/