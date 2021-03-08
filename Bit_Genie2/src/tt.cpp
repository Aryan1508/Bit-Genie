#include "tt.h"
#include "position.h"

static inline int mb_to_b(int mb)
{
  return mb * 1000000;
}

TTable::TTable()
{
  resize(32);
}

void TTable::resize(int mb)
{
  entries.resize(mb_to_b(mb), TEntry());
  printf("Hash table initialized with %d Mb\n", mb);
}

void TTable::add(Position const& position, uint16_t move)
{
  uint64_t hash = position.key.data();
  uint64_t index = hash % entries.size();
  entries[index] = { hash, move };
}

TEntry& TTable::retrieve(Position const& position)
{
  uint64_t hash = position.key.data();
  uint64_t index = hash % entries.size();
  return entries[index];
}