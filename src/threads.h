/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include "search.h"

#include <thread>
#include <memory>

class SearchThreadManager {
public:
    SearchThreadManager() {
        search = std::make_unique<SearchInfo>();
    }

    void start_new_search() {
        if (worker.joinable())
            end();

        search->reset();
        search->limits.stopwatch.go();
        worker = std::thread(bestmove, std::ref(*search), true);
    }

    void end() {
        if (is_searching()) {
            SEARCH_ABORT = true;
            worker.join();
        }
    }

    bool is_searching() const noexcept {
        return worker.joinable();
    }

    void set_limits(SearchLimits const &limits) {
        search->limits = limits;
    }

    auto &get_position() {
        return search->position;
    }

private:
    std::unique_ptr<SearchInfo> search;
    std::thread worker;
};