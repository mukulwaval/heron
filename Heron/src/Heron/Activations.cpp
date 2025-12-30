#include "Heron/Activations.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace Heron {
std::vector<float> Activation::relu(const std::vector<float>& Z) {
  // Initialize
  std::vector<float> A(Z.size());

  // For each element a, return ReLU(a)
  for (size_t i = 0; i < Z.size(); ++i) A[i] = std::max(0.0f, Z[i]);

  return A;
}

std::vector<float> Activation::relu_deriv(const std::vector<float>& Z) {
  // Initialize
  std::vector<float> A(Z.size());

  // For each element a, return ReLU'(a)
  for (size_t i = 0; i < Z.size(); ++i) A[i] = Z[i] > 0 ? 1.0f : 0.0f;

  return A;
}

std::vector<float> Activation::softmax(const std::vector<float>& Z) {
  // Initialize output vector
  std::vector<float> A(Z.size());

  // Find max(x)
  float max_val = *std::max_element(Z.begin(), Z.end());

  // Compute e^{z_i-max(z)}. (z_i-max(z) this is to keep values small.)
  float sum = 0.0f;
  for (float v : Z) sum += std::exp(v - max_val);

  // 0 <= A[i] <= 1
  for (size_t i = 0; i < Z.size(); ++i) A[i] = std::exp(Z[i] - max_val) / sum;

  return A;
}
}  // namespace Heron