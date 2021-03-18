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

void print_cutoffs(Search&);
void search_position(Position&, Search&, TTable& tt);