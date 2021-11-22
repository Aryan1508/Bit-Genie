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
#include "../search.h"
#include "../position.h"

#include <vector>
#include <random>
#include <memory>
#include <fstream>
#include <utility>
#include <string_view>

class Game {
public:
    Game(std::ofstream &output_file, std::mt19937 &rng);

    bool filter_position();

    bool make_book_move();

    void save_position(int search_score);

    bool is_mated() const;

    void save_game(std::string const &result);

    void new_game();

    SearchResult search_position();

    void run();

    std::size_t total_fens_written() const {
        return saved_positions.size();
    }

private:
    int ply = 0;
    SearchInfo search;
    std::ofstream &output_file;
    std::mt19937 &rng;
    std::vector<std::pair<std::string, int>> saved_positions;
};