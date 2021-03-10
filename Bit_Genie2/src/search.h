#pragma once
#include <atomic>
#include "searchinfo.h"
#include "searchlimits.h"
#include "killer.h"
#include "shistory.h"

struct Search
{
  SearchInfo   info;
  SearchLimits limits;
  Killers  killers;
  SHistory history;
};

extern std::atomic_bool SEARCH_ABORT_SIGNAL;

void search_position(Position&, Search&, TTable& tt);