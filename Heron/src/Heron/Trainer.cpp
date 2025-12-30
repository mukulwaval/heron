#include "Heron/Trainer.h"

#include <cstdint>
#include <functional>
#include <iostream>

namespace Heron {
	int Trainer::get_prediction(const std::vector<float>& output)
	{
		int idx = 0;
		float max_val = output[0];

		for (size_t i = 1; i < output.size(); ++i) {
			if (output[i] > max_val) {
				max_val = output[i];
				idx = static_cast<int>(i);
			}
		}
		return idx;
	}

	float Trainer::get_accuracy(const std::vector<int>& predictions,
		const std::vector<uint8_t>& labels)
	{
		const size_t n = labels.size();
		int correct = 0;

		for (size_t i = 0; i < n; ++i)
			correct += (predictions[i] == labels[i]);

		return static_cast<float>(correct) / n;
	}


	void Trainer::gradient_descent(
		Network& net,
		const std::vector<std::vector<float>>& X,
		const std::vector<uint8_t>& Y,
		float alpha,
		int iterations,
		std::function<void(int, float)> callback)
	{
		net.init_params();

		const size_t N = X.size();

		std::vector<int> predictions;
		predictions.resize(N);   // allocate ONCE

		for (int iter = 0; iter < iterations; ++iter) {
			for (size_t i = 0; i < N; ++i) {
				net.forward(X[i]);

				predictions[i] = get_prediction(net.output());

				net.backward(static_cast<int>(Y[i]));
				net.update(alpha);
			}

			float acc = get_accuracy(predictions, Y);
			if (callback) callback(iter, acc);
		}
	}
}  // namespace Heron