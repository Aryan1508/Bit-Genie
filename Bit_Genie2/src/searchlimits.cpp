#include "searchlimits.h"
#include "searchinfo.h"
#include "search.h"
#include "misc.h"

void SearchLimits::start_timer()
{
  start_time = current_time();
}

void SearchLimits::update(SearchInfo const& search)
{
  if (search.nodes & 2047)
  {
    if (time_set && current_time() > stop_time)
      stopped = true;
  }
}