#pragma once
#include "misc.h"

struct SearchLimits
{
  int  max_depth = 1;
  long long start_time;
  long long stop_time;
  bool time_set = false;
  bool stopped = false;

  void update();

  long long time_elapsed() const
  {
    return current_time() - start_time;
  }
};