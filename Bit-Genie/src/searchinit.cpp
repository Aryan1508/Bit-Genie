#include "searchinit.h"
#include "search.h"

void SearchInit::begin(Search &search, Position &position)
{
    if (worker.joinable())
        end();

    using std::ref;
    worker = std::thread(search_position, ref(position), search);
}

void SearchInit::end()
{
    SEARCH_ABORT = true;
    worker.join();
}