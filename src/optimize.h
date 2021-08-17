#pragma once
#include <math.h>

namespace Trainer
{
    constexpr double BETA_1 = 0.9f;
    constexpr double BETA_2 = 0.999f;

    class Parameter
    {
    private:
        double gradient = 0;
        double M = 0, V = 0;

    public:
        Parameter() = default;

        void update_gradient(float delta)
        {
            gradient += delta;
        }

        double get_final_gradient()
        {
            if (!gradient)
                return 0;

            M = M * BETA_1 + gradient * (1 - BETA_1);
            V = V * BETA_2 + (gradient * gradient) * (1 - BETA_2);

            gradient = 0;
            return -0.01 * M / (sqrt(V) + 1e-8);
        }
    };
}