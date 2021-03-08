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

  void add(Position const&, uint16_t);
  TEntry& retrieve(Position const&);
private:
  void resize(int);

private:
  std::vector<TEntry> entries;
};