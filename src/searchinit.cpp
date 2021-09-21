#include "searchinit.h"

SearchInit::SearchInit() {
    search = new Search::Info;
}

SearchInit::~SearchInit() {
    delete search;
}

void SearchInit::begin() {
    if (worker.joinable())
        end();

    using std::ref;
    worker = std::thread(Search::bestmove, ref(*search), true);
}

void SearchInit::end() {
    SEARCH_ABORT = true;
    worker.join();
}