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
    std::cout << "Starting generation on " << FEN_GENERATOR_THREADS << " threads\n";
    std::random_device rd;

    auto generator_start_time   = std::chrono::system_clock::now();
    auto generator_start_time_t = std::chrono::system_clock::to_time_t(generator_start_time);

    std::cout << "Started generation on " << std::ctime(&generator_start_time_t) << std::endl;

    std::uint64_t batch_size = target_fens / FEN_GENERATOR_THREADS;
    for (int i = 0; i < FEN_GENERATOR_THREADS; i++) {
        std::string output_path = "generated_" + std::to_string(i) + ".txt";
        workers.emplace_back(&GamePool::run_batch, this, output_path, batch_size, rd());
    }

    StopWatch watch;
    watch.go();

    while (n_fens <= (batch_size * FEN_GENERATOR_THREADS)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        double fps = n_fens / (double)watch.elapsed_time().count() * 1000;
        std::cout << "\rGenerated fens [ " << n_fens << " ] [ " << fps << " /s ]" << std::flush;
    }
    std::cout << std::endl;

    for (auto &worker : workers)
        worker.join();

    auto generator_end_time   = std::chrono::system_clock::now();
    auto generator_end_time_t = std::chrono::system_clock::to_time_t(generator_end_time);
    std::cout << "Finished generation at " << std::ctime(&generator_end_time_t) << std::endl;
}