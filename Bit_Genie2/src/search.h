#pragma once
#include <atomic>
#include "searchinfo.h"
#include "searchlimits.h"

struct Search
{
  SearchInfo   info;
  SearchLimits limits;
};

extern std::atomic_bool SEARCH_ABORT_SIGNAL;

void search_position(Position&, Search&);