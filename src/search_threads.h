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
#include "board.h"
#include "search.h"

#include <thread>
#include <memory>

class SearchThreadManager {
public:
    SearchThreadManager() {
        set_threads(1);
    }

    void set_threads(size_t count) {
        stop();
        threads.resize(count);
        search_data.resize(count);
    }

    void begin(SearchInfo const &info) {
        if (is_searching())
            stop();

        for (auto &thread_data : search_data) {
            thread_data = info;
        }

        for (size_t i = 0; i < threads.size(); i++) {
            auto is_main_thread = i == 0;
            threads[i]          = std::thread(&search_position, std::ref(search_data[i]), is_main_thread);
        }
    }

    void stop() {
        SEARCH_ABORT = true;

        for (auto &thread : threads) {
            if (thread.joinable())
                thread.join();
        }
    }

    bool is_searching() const {
        return threads[0].joinable();
    }

private:
    std::vector<SearchInfo> search_data;
    std::vector<std::thread> threads;
};