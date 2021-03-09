#pragma once
#include "misc.h"
#include <vector>

struct TEntry
{
  uint64_t hash = 0;
  uint16_t move = 0;
};

class TTable
{
public:
  TTable();

  TTable(int mb) { resize(mb); }

  void resize(int);
  void add(Position const&, uint16_t);
  TEntry& retrieve(Position const&);

private:
  std::vector<TEntry> entries;
};