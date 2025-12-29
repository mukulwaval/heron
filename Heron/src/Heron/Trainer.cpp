#include "Heron/Trainer.h"
#include <cstdint>
#include <iostream>

namespace Heron
{
	int Trainer::get_prediction(const std::vector<float>& output) {
		int idx = 0;
		float max_val = output[0];
		for (int i = 1; i < (int)output.size(); ++i) {
			if (output[i] > max_val) {
				max_val = output[i];
				idx = i;
			}
		}
		return idx;
	}

	float Trainer::get_accuracy(
		const std::vector<int>& predictions,
		const std::vector<uint8_t>& labels
	) {
		int correct = 0;
		for (size_t i = 0; i < labels.size(); ++i) {
			if (predictions[i] == labels[i])
				correct++;
		}
		return static_cast<float>(correct) / labels.size();
	}

	void Trainer::gradient_descent(
		Network& net,
		const std::vector<std::vector<float>>& X,
		const std::vector<uint8_t>& Y,
		float alpha,
		int iterations
	) {
		net.init_params();

		for (int iter = 0; iter < iterations; ++iter) {
			std::vector<int> predictions;
			predictions.reserve(X.size());

			for (size_t i = 0; i < X.size(); ++i) {
				net.forward(X[i]);

				predictions.push_back(get_prediction(net.output()));

				net.backward(static_cast<int>(Y[i]));
				net.update(alpha);
			}

			float acc = get_accuracy(predictions, Y);
			std::cout << "Iteration " << iter << " | Accuracy: " << acc << "\n";
		}
	}
} // namespace Heron