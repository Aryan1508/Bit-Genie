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

#include "Square.h"
#include "activation.h"	
#include "fixed_list.h"

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

        using UpdateArray = FixedList<InputUpdate, 4>;

        Network();

        void feed(NetworkInput const&);

        void update_hidden(UpdateArray const&);

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
            return output_neuron;
        }

        std::vector<std::array<float, HIDDEN_SIZE>> hidden_neurons;

        std::array<std::array<float, HIDDEN_SIZE>, INPUT_SIZE> hidden_weights;
        
        std::array<float, HIDDEN_SIZE> hidden_biases;
        std::array<float, HIDDEN_SIZE> output_weights;

        float output_neuron;
        float output_bias;
    };
}