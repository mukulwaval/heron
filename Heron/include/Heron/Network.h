#pragma once

#include <vector>
#include <fstream>
#include <unordered_map>

#include "Activations.h"
#include "Heron/Core.h"

namespace Heron
{
	class HERON_API Network {
	public:
		// sizes: [input, h1, h2, ..., output]
		// activations: one per layer (excluding input)
		Network(
			const std::vector<size_t>& layer_sizes,
			const std::vector<ActFn>& activations,
			bool initialize = false
		);

		void init_params();

		void forward(const std::vector<float>& input);

		void backward(int Y);

		void update(float learning_rate);

		const std::vector<float>& output() const;

		void save_model(const std::string& file_path) const;

		void load_model(const std::string& file_path);

		void predict(const std::vector<float>& input,
			std::vector<std::vector<float>>& Z_out,
			std::vector<std::vector<float>>& A_out) const;

	private:
		// weights[i] : layer i -> i+1
		std::vector<std::vector<std::vector<float>>> weights;
		std::vector<std::vector<float>> biases;

		std::vector<std::vector<std::vector<float>>> dW;
		std::vector<std::vector<float>> db;

		std::vector<size_t> layer_sizes;

		std::vector<std::vector<float>> Z; // pre-activation
		std::vector<std::vector<float>> A; // activations

		std::vector<ActFn> activation_fns;
		std::vector<ActFn> activation_derivs;
	};
} // namespace Heron
