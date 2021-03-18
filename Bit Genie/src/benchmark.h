#pragma once
#include "misc.h"

namespace BenchMark
{
	void perft(Position&, int depth);
	void bench(Position, TTable&);
}