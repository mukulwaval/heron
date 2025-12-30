#pragma once
#include <functional>
#include <vector>

#include "Core.h"
#include "Network.h"

namespace Heron {
class HERON_API Trainer {
 public:
  static int get_prediction(const std::vector<float>& output);
  static float get_accuracy(const std::vector<int>& predictions,
                            const std::vector<uint8_t>& labels);
  static void gradient_descent(
      Network& net, const std::vector<std::vector<float>>& X,
      const std::vector<uint8_t>& Y, float alpha, int iterations,
      std::function<void(int, float)> callback = nullptr);
};
}  // namespace Heron
