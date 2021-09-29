#include "searchinit.h"

SearchInit::SearchInit() {
    search = std::make_unique<SearchInfo>();
}

void SearchInit::begin() {
    if (worker.joinable())
        end();
    worker = std::thread(search_position, std::ref(*search), true);
}

void SearchInit::end() {
    SEARCH_ABORT = true;
    worker.join();
}