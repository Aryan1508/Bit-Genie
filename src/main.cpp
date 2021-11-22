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
#include "uci.h"
#include "search.h"
#include "attacks.h"
#include "zobrist.h"
#include "network.h"
#include "fen-gen/generator.h"

int main(int argc, char **argv) {
    init_magics();
    init_zobrist_keys();
    init_search_tables();
    Network::init();

#ifndef FEN_GENERATOR 
    init_uci(argc, argv);
#else
const std::string HELP_TEXT = R"(
    ./Bit-Genie-generator -threads <number of threads to use>  (default 1) 
                          -depth   <search depth limi>         (default 10) 
                          -nodes   <search node limit>         (default 4000) 
                          -fens   <number of fens to generate> (default 32768)

)";
    std::cout << HELP_TEXT << '\n';
    std::cout << "Bit-Genie Fen Generator";
    std::cout << " [version=" << BG_VERSION << ", network=" << std::hex << Network::get_hash() << std::dec << ".nn]\n";

    CommandLineParser cmdline(argc, argv);

    FEN_GENERATOR_THREADS = cmdline.get_option("-threads", 1);
    FEN_GENERATOR_DEPTH   = cmdline.get_option("-depth",  10);
    FEN_GENERATOR_NODES   = cmdline.get_option("-nodes",  4000);

    std::cout << "Configuration           [";
    std::cout << "threads=" << FEN_GENERATOR_THREADS << ", ";
    std::cout << "depth=" << FEN_GENERATOR_DEPTH << ", ";
    std::cout << "nodes=" << FEN_GENERATOR_NODES << "]\n";
    std::cout << std::endl;

    GamePool pool;
    pool.run(cmdline.get_option("-fens", 32768));
#endif
}