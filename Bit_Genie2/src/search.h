#pragma once
#include <atomic>
#include "searchinfo.h"
#include "searchlimits.h"
#include "killer.h"

struct Search
{
  SearchInfo   info;
  SearchLimits limits;
  Killers      killers;
};

extern std::atomic_bool SEARCH_ABORT_SIGNAL;

void search_position(Position&, Search&);