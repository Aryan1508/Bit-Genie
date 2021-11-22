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
#include "game.h"

#define DRAW_STRING      ("[0.5]")
#define WHITE_WIN_STRING ("[1.0]")
#define BLACK_WIN_STRING ("[0.0]")

Game::Game(std::ofstream &output_file, std::mt19937 &rng)
    : output_file(output_file), rng(rng) {}

bool Game::filter_position() {
    return (qsearch(search, MIN_EVAL, MAX_EVAL) == search.position.static_evaluation()) && !search.position.king_in_check();
}

bool Game::make_book_move() {
    if (is_mated())
        return false;

    Movelist movelist;
    search.position.generate_legal(movelist);

    std::uniform_int_distribution distribution(0, static_cast<int>(movelist.size() - 1));
    int rand_index = distribution(rng);

    search.position.apply_move(movelist[rand_index]);
    ply++;
    return true;
}

void Game::save_position(int search_score) {
    saved_positions.push_back({ search.position.get_fen(), search_score });
}

bool Game::is_mated() const {
    Movelist movelist;
    search.position.generate_legal(movelist);
    return movelist.size() == 0;
}

void Game::new_game() {
    ply = 0;
    search.position.set_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    saved_positions.clear();
    search.reset_for_search();
}

void Game::save_game(std::string const &result) {
    for (auto const &saved_position : saved_positions) {
        std::string line = saved_position.first + " " + result + " " + std::to_string(saved_position.second);
        output_file << line << '\n';
    }
}

SearchResult Game::search_position() {
    return ::search_position(search, false);
}

void Game::run() {
    std::string result;
    int draw_score_counter = 0, win_score_counter = 0;

    while (true) {
        /* TODO: command line arg */
        if (ply < 8) {
            if (!make_book_move())
                break;
            continue;
        }

        if (search.position.drawn()) {
            result = DRAW_STRING;
            break;
        }

        if (is_mated()) {
            if (search.position.king_in_check())
                result = search.position.get_side() == CLR_WHITE ? BLACK_WIN_STRING : WHITE_WIN_STRING;
            else
                result = DRAW_STRING;
            break;
        }

        auto [score, move]       = search_position();
        int white_relative_score = search.position.get_side() == CLR_WHITE ? score : -score;

        // TODO command line arg
        if (score >= 1000 && ply == 8)
            return;

        if (filter_position())
            save_position(white_relative_score);

        // TODO command line arg
        bool is_draw_score = std::abs(score) <= 20;
        bool is_win_score  = std::abs(score) >= 1000;

        draw_score_counter = is_draw_score ? draw_score_counter + 1 : 0;
        win_score_counter  = is_win_score ? win_score_counter + 1 : 0;

        // TODO command line arg
        if (draw_score_counter >= 12) {
            result = DRAW_STRING;
            break;
        }

        if (win_score_counter >= 4) {
            result = white_relative_score > 0 ? WHITE_WIN_STRING : BLACK_WIN_STRING;
            break;
        }

        search.position.apply_move(move);
        ply++;
    }
    save_game(result);
}