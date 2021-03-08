#include "searchlimits.h"
#include "search.h"
#include "misc.h"

void SearchLimits::update()
{
  if (SEARCH_ABORT_SIGNAL)
    stopped = true;

  if (time_set && current_time() > stop_time)
    stopped = true;
}