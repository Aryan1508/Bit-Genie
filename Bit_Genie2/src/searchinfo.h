#pragma once
#include "misc.h"

struct SearchInfo
{

	uint64_t total_nodes = 0;
	uint64_t nodes = 0;
	uint64_t total_cutoffs = 0;
	int      ply = 0;
	int      depth = 0;
	int      seldepth = 0;

	void update_seldepth()
	{
		seldepth = std::max(seldepth, ply);
	}
};