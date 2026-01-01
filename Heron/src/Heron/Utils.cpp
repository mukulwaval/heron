#include "Heron/Utils.h"

#include <omp.h>

#include <cstring>

namespace Heron {
std::vector<float> Utils::mat_vec(const std::vector<std::vector<float>>& mat,
                                  const std::vector<float>& vec,
                                  const std::vector<float>& bias) {
  std::vector<float> result(mat.size());
  mat_vec_inplace(mat, vec, bias, result);
  return result;
}

std::vector<float> Utils::matT_vec(const std::vector<std::vector<float>>& M,
                                   const std::vector<float>& x) {
  std::vector<float> out(M[0].size());
  matT_vec_inplace(M, x, out);
  return out;
}

void Utils::outer_product(const std::vector<float>& a,
                          const std::vector<float>& b,
                          std::vector<std::vector<float>>& out) {
  outer_product_inplace(a, b, out);
}

// one_hot: no major optimization needed, just resize once
std::vector<float> Utils::one_hot(int label, int classes) {
  std::vector<float> oh(classes, 0.0f);
  oh[label] = 1.0f;
  return oh;
}

void Utils::mat_vec_inplace(const std::vector<std::vector<float>>& mat,
                            const std::vector<float>& vec,
                            const std::vector<float>& bias,
                            std::vector<float>& out) {
  const size_t rows = mat.size();
  const size_t cols = mat[0].size();

  if (out.size() != rows) out.resize(rows);

#pragma omp parallel for
  for (size_t i = 0; i < rows; ++i) {
    float sum = bias[i];
    const auto& row = mat[i];
#pragma omp simd reduction(+ : sum)
    for (size_t j = 0; j < cols; ++j) sum += row[j] * vec[j];
    out[i] = sum;
  }
}

void Utils::matT_vec_inplace(const std::vector<std::vector<float>>& M,
                             const std::vector<float>& x,
                             std::vector<float>& out) {
  const size_t rows = M.size();
  const size_t cols = M[0].size();

  if (out.size() != cols)
    out.assign(cols, 0.0f);
  else
    std::fill(out.begin(), out.end(), 0.0f);

  for (size_t i = 0; i < rows; ++i) {
    const float xi = x[i];
    const auto& row = M[i];
#pragma omp simd
    for (size_t j = 0; j < cols; ++j) out[j] += row[j] * xi;
  }
}

void Utils::outer_product_inplace(const std::vector<float>& a,
                                  const std::vector<float>& b,
                                  std::vector<std::vector<float>>& out) {
  const size_t rows = a.size();
  const size_t cols = b.size();

  if (out.size() != rows) out.resize(rows);
  for (size_t i = 0; i < rows; ++i)
    if (out[i].size() != cols) out[i].resize(cols);

#pragma omp parallel for
  for (size_t i = 0; i < rows; ++i) {
    const float ai = a[i];
#pragma omp simd
    for (size_t j = 0; j < cols; ++j) out[i][j] = ai * b[j];
  }
}

}  // namespace Heron
