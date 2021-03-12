#pragma once
#include "misc.h"
#include <chrono>

struct SearchLimits
{
  using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

  time_point start_time;
  time_point stop_time;
  volatile bool time_set = false;
  volatile bool stopped = false;

  int  max_depth = 1;

  void update(SearchInfo const&);
  void start_timer();
  auto time_elapsed()
  {
    using namespace std::chrono;
    return duration_cast<milliseconds>(current_time() - start_time).count();
  }
};