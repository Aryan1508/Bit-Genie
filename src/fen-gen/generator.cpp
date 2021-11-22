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
#include "generator.h"

#include <chrono>
#include <time.h>
#include <iomanip>
#include <sstream>

std::string get_current_date_time() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

void GamePool::run_batch(std::string_view output_file_path, std::uint64_t target_fens, std::uint64_t seed) {
    std::mt19937 rng(seed);
    std::ofstream output_file(output_file_path.data());

    if (!output_file) {
        std::cerr << "Couldn't open " << output_file_path << std::endl;
        std::terminate();
    }

    Game game(output_file, rng);

    for (std::uint64_t n_batch_fens = 0; n_batch_fens <= target_fens;) {
        game.run();
        n_games++;

        n_batch_fens += game.total_fens_written();
        n_fens += game.total_fens_written();
        game.new_game();
    }
    output_file.close();
}

void GamePool::run(std::uint64_t target_fens) {
    std::random_device rd;

    std::uint64_t batch_size = target_fens / FEN_GENERATOR_THREADS;
    for (int i = 0; i < FEN_GENERATOR_THREADS; i++) {
        std::string output_path = "generated_" + std::to_string(i) + ".txt";
        workers.emplace_back(&GamePool::run_batch, this, output_path, batch_size, rd());
    }

    std::cout << "\n{" << get_current_date_time() << "}: started generation\n";

    std::uint64_t n_previous_fens = 0;
    while (n_fens <= (batch_size * FEN_GENERATOR_THREADS)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::uint64_t n_batch_fens = n_fens - n_previous_fens;
        std::cout << "\r{" << get_current_date_time() << "}: generating... [total=" << n_fens << ", speed=" << n_batch_fens << "]" << std::flush; 
        n_previous_fens = n_fens;
    }
    std::cout << '\n';
    std::cout << "{" << get_current_date_time() << "}: finished generation" << std::endl;

    for (auto &worker : workers)
        worker.join();
}