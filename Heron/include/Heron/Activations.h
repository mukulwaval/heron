#pragma once
#include <vector>

#include "Core.h"

using ActFn = std::vector<float> (*)(const std::vector<float>&);

namespace Heron {
struct HERON_API Activation {
  static std::vector<float> relu(const std::vector<float>& Z);
  static std::vector<float> relu_deriv(const std::vector<float>& Z);
  static std::vector<float> tanh(const std::vector<float>& Z);
  static std::vector<float> tanh_deriv(const std::vector<float>& Z);
  static std::vector<float> softmax(const std::vector<float>& Z);

  static void relu_inplace(const std::vector<float>& Z, std::vector<float>& out);
  static void relu_deriv_inplace(const std::vector<float>& Z, std::vector<float>& out);
  static void tanh_inplace(const std::vector<float>& Z, std::vector<float>& out);
  static void tanh_deriv_inplace(const std::vector<float>& Z, std::vector<float>& out);
  static void softmax_inplace(const std::vector<float>& Z, std::vector<float>& out);
};
}  // namespace Heron
