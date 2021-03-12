#pragma once
#include "misc.h"
#include "stopwatch.h"
#include <chrono>

struct SearchLimits
{
  StopWatch<std::chrono::milliseconds> stopwatch;
  int64_t movetime = -1;
  bool    time_set = false;
  bool    stopped = false;
  int     max_depth = 1;

  void update()
  {
    stopped = stopwatch.elapsed_time().count() >= movetime;
  }
};