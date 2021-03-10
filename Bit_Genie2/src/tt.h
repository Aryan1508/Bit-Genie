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
    std::fill(entries.begin(), entries.end(), TEntry());
  }

  TEntry& retrieve(Position const&);

private:
  std::vector<TEntry> entries;
};