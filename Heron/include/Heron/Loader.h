#pragma once
#include <string>
#include <vector>
#include <array>
#include "Core.h"

namespace Heron {
	class HERON_API Loader {
	public:
		explicit Loader(const std::string& dataset_path);
		~Loader();

		bool load(const std::string& split);

		size_t size() const;
		const std::vector<float>& get_image(size_t idx) const;
		uint8_t get_label(size_t idx) const;

	private:
		std::string path;
		std::vector<std::vector<float>> images;
		std::vector<uint8_t> labels;
	};
} // namespace Heron
