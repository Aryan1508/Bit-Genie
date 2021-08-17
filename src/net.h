#pragma once
#include <vector>

#include "Square.h"
#include "matrix.h"
#include "optimize.h"
#include "activation.h"	

namespace Trainer
{
    struct NetworkInput
    {
        std::vector<int> activated_input_indices;
        float target = 0;
    };

    inline uint16_t calculate_input_index(Square sq, Piece piece)
    {
        return piece * 64 + sq;
    }

    struct InputUpdate
    {
        enum : int8_t { Addition = 1, Removal = -1 };

        uint16_t index;
        int8_t   coeff; 

        InputUpdate() = default;

        InputUpdate(Square sq, Piece piece, int8_t coeff)
            : index(calculate_input_index(sq, piece)), coeff(coeff)
        {}
    };

    class Network
    {
    public:
        static constexpr int INPUT_SIZE  = 768;
        static constexpr int HIDDEN_SIZE = 128;
        static constexpr int OUTPUT_SIZE = 1;

        Network();

        void feed(NetworkInput const&);

        void update_hidden(std::vector<InputUpdate> const&);

        void revert_hidden_updates()
        {
            hidden_neurons.pop_back();
        }

        void recalculate_hidden(NetworkInput const& input)
        {
            hidden_neurons.clear();
            hidden_neurons.push_back({});
            feed(input);
        }

        float quick_feed();

        float get_output() const
        {
            return output_neuron.get(0);
        }

        std::vector<Matrix<float, HIDDEN_SIZE, 1>> hidden_neurons;
        Matrix<float, OUTPUT_SIZE, 1> output_neuron;

        Matrix<float, OUTPUT_SIZE, 1> output_bias;
        Matrix<float, HIDDEN_SIZE, 1> hidden_biases;

        Matrix<float, HIDDEN_SIZE, INPUT_SIZE, Arrangement::ColMajor>   hidden_weights;
        Matrix<float, OUTPUT_SIZE, HIDDEN_SIZE>  output_weights;
    };
}