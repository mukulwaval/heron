#pragma once
#include "Heron/Network.h"
#include "FileIO.h"
#include <iostream>
#include <string>

namespace Heron {
	class HERON_API Inference {
	public:
		// Returns the predicted class for a single input
		static int make_prediction(const Heron::Network& net, const std::vector<float>& X);

		// Test a sample and optionally save it to disk
		static void test_prediction(
			const Heron::Network& net,
			const std::vector<std::vector<float>>& X,
			const std::vector<uint8_t>& Y,
			size_t index,
			const std::string& out_file = "sample.bmp"
		);

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
