#pragma once
#include <math.h>
#include <algorithm>

namespace Trainer
{
    constexpr float sigmoid_scale = 3.0f / 400.0f;

    inline float sigmoid(float x)
    {
        return 1.0f / (1.0f + expf(-sigmoid_scale * x));
    }

    inline float sigmoid_prime(float x)
    {
        return x * (1.0f - x) * sigmoid_scale;
    }

    inline float relu(float x)
    {
        return std::max<float>(x, 0.0f);
    }
}