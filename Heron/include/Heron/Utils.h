#pragma once
#include <vector>

#include "Core.h"

namespace Heron {
class HERON_API Utils {
 public:
  static std::vector<float> mat_vec(const std::vector<std::vector<float>>& mat,
                                    const std::vector<float>& vec,
                                    const std::vector<float>& bias);
  static std::vector<float> matT_vec(const std::vector<std::vector<float>>& M,
                                     const std::vector<float>& x);
  static void outer_product(const std::vector<float>& a,
                            const std::vector<float>& b,
                            std::vector<std::vector<float>>& out);
  static std::vector<float> one_hot(int label, int classes);
  static void mat_vec_inplace(const std::vector<std::vector<float>>& mat,
                              const std::vector<float>& vec,
                              const std::vector<float>& bias,
                              std::vector<float>& out);
  static void matT_vec_inplace(const std::vector<std::vector<float>>& M,
                               const std::vector<float>& x,
                               std::vector<float>& out);
  static void outer_product_inplace(const std::vector<float>& a,
                                    const std::vector<float>& b,
                                    std::vector<std::vector<float>>& out);
};
}  // namespace Heron