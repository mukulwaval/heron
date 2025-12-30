#include "Heron/Inference.h"

namespace Heron {
int Inference::make_prediction(const Heron::Network& net,
                               const std::vector<float>& X) {
  std::vector<std::vector<float>> Z, A;
  net.predict(X, Z, A);
  return argmax(A.back());  // last layer activation
}

void Inference::test_prediction(const Heron::Network& net,
                                const std::vector<std::vector<float>>& X,
                                const std::vector<uint8_t>& Y, size_t index,
                                const std::string& out_file) {
  const auto& image = X[index];
  int prediction = make_prediction(net, image);

  std::cout << "Prediction: " << prediction << "\n";
  std::cout << "Label:      " << static_cast<int>(Y[index]) << "\n";

  FileIO::write_28x28_bmp(image, out_file);
  std::cout << "Saved image to " << out_file << "\n";
}
}  // namespace Heron