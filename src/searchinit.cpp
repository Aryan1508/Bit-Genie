#include "searchinit.h"

SearchInit::SearchInit() {
    search = new SearchInfo;
}

SearchInit::~SearchInit() {
    delete search;
}

void SearchInit::begin() {
    if (worker.joinable())
        end();

    using std::ref;
    worker = std::thread(bestmove, ref(*search), true);
}

void SearchInit::end() {
    SEARCH_ABORT = true;
    worker.join();
}