#include "net.h"
#include "../incbin/incbin.h"
#include <thread>
#include <vector>
#include <cstring>

INCBIN(Network, EVALFILE);

namespace 
{
    template<int input_neuron_count, typename T> 
    void randomize_matrix(T& matrix)
    {
        float g = 2 / sqrtf(static_cast<float>(input_neuron_count));

        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> distrib(0.0f, g);

        for(int i = 0;i < matrix.size();i++)
            matrix.get(i) = distrib(gen);
    }
}

namespace Trainer
{
    Network::Network()
    {
        hidden_neurons.push_back({});
        hidden_neurons.back().set(0.0f);
        output_neuron .set(0.0f);

        auto data = reinterpret_cast<const float*>(gNetworkData) + 1;

        for(int i = 0;i < hidden_weights.size();i++) 
            hidden_weights.get(i) = *data++;

        for(int i = 0;i < output_weights.size();i++) 
            output_weights.get(i) = *data++;

        for(int i = 0;i < hidden_biases.size();i++) 
            hidden_biases.get(i) = *data++;

        output_bias.get(0) = *data++;
    }

    void Network::update_hidden(std::vector<InputUpdate> const& updates)
    {
        hidden_neurons.push_back(hidden_neurons.back());

        for(auto const& update : updates)
        {
            for(int i = 0;i < hidden_neurons.back().size();i++)
                hidden_neurons.back().get(i) += update.coeff * hidden_weights.get(i, update.index);
        }
    }

    float Network::quick_feed()
    {
        output_neuron.set(0.0f);

        for (int k = 0; k < output_weights.total_cols(); k++)
            output_neuron.get(0) +=  relu(hidden_neurons.back().get(k)) * output_weights.get(k);

        output_neuron.get(0) += output_bias.get(0);
        return get_output();
    }

    void Network::feed(NetworkInput const& sample)
    {
        hidden_neurons.back().set(0.0f);

        for (auto index : sample.activated_input_indices)
        {
            for (int i = 0; i < hidden_weights.total_rows(); i++)
                hidden_neurons.back().get(i) += hidden_weights.get(i, index);
        }

        for (int i = 0; i < hidden_neurons.back().size(); i++)
            hidden_neurons.back().get(i) += hidden_biases.get(i);

        quick_feed();
    }
}