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
#include "incbin/incbin.h"
#include "net.h"
#include <vector>
#include <cstring>

INCBIN(Network, EVALFILE);

namespace Trainer
{
    Network::Network()
    {
        hidden_neurons.push_back({});
        hidden_neurons.back().fill(0.0f);
        output_neuron = 0.0f;

        auto data = reinterpret_cast<const float*>(gNetworkData) + 1;

        for(std::size_t i = 0;i < hidden_weights.size();i++) 
            for(std::size_t j = 0;j < hidden_weights[i].size();j++)
                hidden_weights[i][j] = *data++;

        for(std::size_t i = 0;i < output_weights.size();i++) 
            output_weights[i] = *data++;

        for(std::size_t i = 0;i < hidden_biases.size();i++) 
            hidden_biases[i] = *data++;

        output_bias = *data++;
    }

    void Network::update_hidden(NetworkUpdateList const& updates)
    {
        hidden_neurons.push_back(hidden_neurons.back());

        for(auto const& update : updates)
        {
            for(std::size_t i = 0;i < hidden_neurons.back().size();i++)
                hidden_neurons.back()[i] += update.coeff * hidden_weights[update.index][i];
        }
    }

    float Network::quick_feed()
    {
        output_neuron = 0.0f;

        for (std::size_t k = 0; k < output_weights.size(); k++)
            output_neuron +=  std::max<float>(hidden_neurons.back()[k], 0.0f) * output_weights[k];

        output_neuron += output_bias;
        return get_output();
    }

    void Network::feed(NetworkInput const& sample)
    {
        hidden_neurons.back().fill(0);

        for (auto index : sample.activated_input_indices)
        {
            for (std::size_t i = 0; i < HIDDEN_SIZE; i++)
                hidden_neurons.back()[i] += hidden_weights[index][i];
        }

        for (std::size_t i = 0; i < hidden_neurons.back().size(); i++)
            hidden_neurons.back()[i] += hidden_biases[i];

        quick_feed();
    }
}