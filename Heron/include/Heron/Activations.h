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
};
}  // namespace Heron
