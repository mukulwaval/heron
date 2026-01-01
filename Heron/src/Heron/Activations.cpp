#include "Heron/Activations.h"

#include <omp.h>

#include <algorithm>
#include <cmath>
#include <vector>

namespace Heron {
std::vector<float> Activation::relu(const std::vector<float>& Z) {
  std::vector<float> out(Z.size());
  relu_inplace(Z, out);
  return out;
}

std::vector<float> Activation::relu_deriv(const std::vector<float>& Z) {
  std::vector<float> out(Z.size());
  relu_deriv_inplace(Z, out);
  return out;
}

std::vector<float> Activation::tanh(const std::vector<float>& Z) {
  std::vector<float> out(Z.size());
  tanh_inplace(Z, out);
  return out;
}

std::vector<float> Activation::tanh_deriv(const std::vector<float>& Z) {
  std::vector<float> out(Z.size());
  tanh_deriv_inplace(Z, out);
  return out;
}

std::vector<float> Activation::softmax(const std::vector<float>& Z) {
  std::vector<float> out(Z.size());
  softmax_inplace(Z, out);
  return out;
}

void Activation::relu_inplace(const std::vector<float>& Z,
                              std::vector<float>& out) {
  const size_t n = Z.size();
  if (out.size() != n) out.resize(n);

#pragma omp simd
  for (size_t i = 0; i < n; ++i) out[i] = Z[i] > 0.0f ? Z[i] : 0.0f;
}

void Activation::relu_deriv_inplace(const std::vector<float>& Z,
                                    std::vector<float>& out) {
  const size_t n = Z.size();
  if (out.size() != n) out.resize(n);

#pragma omp simd
  for (size_t i = 0; i < n; ++i) out[i] = Z[i] > 0.0f ? 1.0f : 0.0f;
}

void Activation::tanh_inplace(const std::vector<float>& Z,
                              std::vector<float>& out) {
  const size_t n = Z.size();
  if (out.size() != n) out.resize(n);

#pragma omp simd
  for (size_t i = 0; i < n; ++i) out[i] = std::tanh(Z[i]);
}

void Activation::tanh_deriv_inplace(const std::vector<float>& Z,
                                    std::vector<float>& out) {
  const size_t n = Z.size();
  if (out.size() != n) out.resize(n);

#pragma omp simd
  for (size_t i = 0; i < n; ++i) {
    float t = std::tanh(Z[i]);
    out[i] = 1.0f - t * t;
  }
}
void Activation::softmax_inplace(const std::vector<float>& Z,
                                 std::vector<float>& out) {
  const size_t n = Z.size();
  if (out.size() != n) out.resize(n);

  float max_val = *std::max_element(Z.begin(), Z.end());
  float sum = 0.0f;

#pragma omp parallel for reduction(+ : sum)
  for (size_t i = 0; i < n; ++i) {
    out[i] = std::exp(Z[i] - max_val);
    sum += out[i];
  }

  const float inv_sum = 1.0f / sum;
#pragma omp simd
  for (size_t i = 0; i < n; ++i) out[i] *= inv_sum;
}

}  // namespace Heron
