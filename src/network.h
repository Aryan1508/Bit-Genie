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
#include <vector>

#include "net_input.h"
#include "fixed_list.h"

class Network {
public:
    static constexpr int INPUT_SIZE  = 768;
    static constexpr int HIDDEN_SIZE = 256;
    static constexpr int OUTPUT_SIZE = 1;

    Network() {
        hidden_neurons.push_back({});
        hidden_neurons.back().fill(0.0f);
    }

    int16_t calculate_last_layer();

    int16_t feed(NetworkInput const &);

    void update_hidden_layer(NetworkUpdateList const &);

    void revert_hidden_updates() {
        hidden_neurons.pop_back();
    }

    void recalculate_hidden_layer(NetworkInput const &input) {
        hidden_neurons.clear();
        hidden_neurons.push_back({});
        feed(input);
    }

    static void init();

    static std::uint32_t get_hash() {
        return hash;
    }

private:
    std::vector<std::array<int16_t, HIDDEN_SIZE>> hidden_neurons;

    static std::array<std::array<int16_t, HIDDEN_SIZE>, INPUT_SIZE> hidden_weights;
    static std::array<int16_t, HIDDEN_SIZE> hidden_biases;
    static std::array<int16_t, HIDDEN_SIZE> output_weights;
    static int16_t output_bias;
    static std::uint32_t hash;
};