#include "searchinit.h"

void SearchInit::begin(Search::Info& search)
{
    if (worker.joinable())
        end();

    using std::ref;
    worker = std::thread(Search::bestmove, search, true);
}

void SearchInit::end()
{
    SEARCH_ABORT = true;
    worker.join();
}