#include "Heron/Activations.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace Heron {
    std::vector<float> Activation::relu(const std::vector<float>& Z)
    {
        const size_t n = Z.size();
        std::vector<float> A;
        A.resize(n);

        for (size_t i = 0; i < n; ++i)
            A[i] = Z[i] > 0.0f ? Z[i] : 0.0f;

        return A;
    }

    std::vector<float> Activation::relu_deriv(const std::vector<float>& Z)
    {
        const size_t n = Z.size();
        std::vector<float> A;
        A.resize(n);

        for (size_t i = 0; i < n; ++i)
            A[i] = Z[i] > 0.0f ? 1.0f : 0.0f;

        return A;
    }

    std::vector<float> Activation::tanh(const std::vector<float>& Z)
    {
        const size_t n = Z.size();
        std::vector<float> A;
        A.resize(n);

        for (size_t i = 0; i < n; ++i)
            A[i] = std::tanh(Z[i]);

        return A;
    }

    std::vector<float> Activation::tanh_deriv(const std::vector<float>& Z)
    {
        const size_t n = Z.size();
        std::vector<float> A;
        A.resize(n);

        for (size_t i = 0; i < n; ++i) {
            float t = std::tanh(Z[i]);
            A[i] = 1.0f - t * t;   // tanh'(x) = 1 - tanh²(x)
        }

        return A;
    }

    std::vector<float> Activation::softmax(const std::vector<float>& Z)
    {
        const size_t n = Z.size();
        std::vector<float> A;
        A.resize(n);

        float max_val = *std::max_element(Z.begin(), Z.end());

        float sum = 0.0f;
        for (size_t i = 0; i < n; ++i)
            sum += std::exp(Z[i] - max_val);

        const float inv_sum = 1.0f / sum;
        for (size_t i = 0; i < n; ++i)
            A[i] = std::exp(Z[i] - max_val) * inv_sum;

        return A;
    }

} // namespace Heron
