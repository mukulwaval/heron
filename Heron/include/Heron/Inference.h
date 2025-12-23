#pragma once
#include "Heron/Network.h"
#include "FileIO.h"
#include <iostream>
#include <string>

namespace Heron {
	class HERON_API Inference {
	public:
		// Returns the predicted class for a single input
		static int make_prediction(const Heron::Network& net, const std::vector<float>& X) {
			std::vector<std::vector<float>> Z, A;
			net.predict(X, Z, A);
			return argmax(A.back()); // last layer activation
		}

		// Test a sample and optionally save it to disk
		static void test_prediction(
			const Heron::Network& net,
			const std::vector<std::vector<float>>& X,
			const std::vector<uint8_t>& Y,
			size_t index,
			const std::string& out_file = "sample.bmp"
		) {
			const auto& image = X[index];
			int prediction = make_prediction(net, image);

			std::cout << "Prediction: " << prediction << "\n";
			std::cout << "Label:      " << static_cast<int>(Y[index]) << "\n";

			FileIO::write_28x28_bmp(image, out_file);
			std::cout << "Saved image to " << out_file << "\n";
		}

	private:
		static int argmax(const std::vector<float>& v) {
			int idx = 0;
			float max_val = v[0];
			for (size_t i = 1; i < v.size(); ++i) {
				if (v[i] > max_val) {
					max_val = v[i];
					idx = static_cast<int>(i);
				}
			}
			return idx;
		}
	};
} // namespace Heron
