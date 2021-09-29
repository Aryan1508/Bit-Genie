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
#include "network.h"
#include "incbin/incbin.h"

#include <vector>
#include <cstring>

INCBIN(Network, EVALFILE);

void Network::init() {
    auto data = reinterpret_cast<const uint32_t *>(gNetworkData);
    hash      = *(data + 1);

    data += 6;
    std::memcpy(&hidden_weights[0], data, HIDDEN_SIZE * INPUT_SIZE * sizeof(float));
    data += HIDDEN_SIZE * INPUT_SIZE;

    std::memcpy(&hidden_biases[0], data, HIDDEN_SIZE * sizeof(float));
    data += HIDDEN_SIZE;

    std::memcpy(&output_weights[0], data, HIDDEN_SIZE * sizeof(float));
    data += HIDDEN_SIZE;

    std::memcpy(&output_bias, data, sizeof(float));
}

void Network::update_hidden_layer(NetworkUpdateList const &updates) {
    hidden_neurons.push_back(hidden_neurons.back());

    for (auto const &update : updates) {
        for (std::size_t i = 0; i < hidden_neurons.back().size(); i++)
            hidden_neurons.back()[i] += update.coeff * hidden_weights[update.index][i];
    }
}

float Network::calculate_last_layer() {
    auto sum = 0.0f;

    for (std::size_t k = 0; k < output_weights.size(); k++)
        sum += std::max<float>(hidden_neurons.back()[k], 0.0f) * output_weights[k];

    return sum + output_bias;
}

float Network::feed(NetworkInput const &sample) {
    hidden_neurons.back().fill(0);

    for (auto index : sample) {
        for (std::size_t i = 0; i < HIDDEN_SIZE; i++)
            hidden_neurons.back()[i] += hidden_weights[index][i];
    }

    for (std::size_t i = 0; i < hidden_neurons.back().size(); i++)
        hidden_neurons.back()[i] += hidden_biases[i];

    return calculate_last_layer();
}

std::array<std::array<float, Network::HIDDEN_SIZE>, Network::INPUT_SIZE> Network::hidden_weights;
std::array<float, Network::HIDDEN_SIZE> Network::hidden_biases;
std::array<float, Network::HIDDEN_SIZE> Network::output_weights;
float Network::output_bias;
std::uint32_t Network::hash;