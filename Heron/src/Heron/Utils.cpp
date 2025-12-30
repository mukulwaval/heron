#include "Heron/Utils.h"

namespace Heron {
std::vector<float> Utils::mat_vec(const std::vector<std::vector<float>>& mat,
                                  const std::vector<float>& vec,
                                  const std::vector<float>& bias) {
  const size_t rows = mat.size();
  const size_t cols = mat[0].size();

  std::vector<float> result;
  result.resize(rows);  // allocate once

  for (size_t i = 0; i < rows; ++i) {
    float sum = bias[i];
    const auto& row = mat[i];

    for (size_t j = 0; j < cols; ++j) sum += row[j] * vec[j];

    result[i] = sum;
  }
  return result;
}

std::vector<float> Utils::matT_vec(const std::vector<std::vector<float>>& M,
                                   const std::vector<float>& x) {
  const size_t rows = M.size();
  const size_t cols = M[0].size();

  std::vector<float> out;
  out.assign(cols, 0.0f);  // one allocation

  for (size_t i = 0; i < rows; ++i) {
    const auto& row = M[i];
    const float xi = x[i];

    for (size_t j = 0; j < cols; ++j) out[j] += row[j] * xi;
  }
  return out;
}

void Utils::outer_product(const std::vector<float>& a,
                          const std::vector<float>& b,
                          std::vector<std::vector<float>>& out) {
  const size_t rows = a.size();
  const size_t cols = b.size();

  // resize, don't reassign
  out.resize(rows);
  for (size_t i = 0; i < rows; ++i) out[i].resize(cols);

  for (size_t i = 0; i < rows; ++i) {
    const float ai = a[i];
    for (size_t j = 0; j < cols; ++j) out[i][j] = ai * b[j];
  }
}

std::vector<float> Utils::one_hot(int label, int classes) {
  std::vector<float> oh;
  oh.assign(classes, 0.0f);
  oh[label] = 1.0f;
  return oh;
}
}  // namespace Heron