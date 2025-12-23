#pragma once
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <cstdint>
#include <iostream>

#define TRAIN_SIZE 60000
#define TEST_SIZE 10000

namespace Heron {
	class HERON_API Loader {
	public:
		static constexpr int WIDTH = 28;
		static constexpr int HEIGHT = 28;
		static constexpr int IMAGE_SIZE = WIDTH * HEIGHT;

		explicit Loader(const std::string& dataset_path)
			: path(dataset_path) {
		}

		bool load(const std::string& split) {
			std::string img_file = path + "/" + split + "_images.bin";
			std::string lbl_file = path + "/" + split + "_labels.bin";

			std::ifstream img_in(img_file, std::ios::binary);
			std::ifstream lbl_in(lbl_file, std::ios::binary);

			if (!img_in || !lbl_in) {
				std::cerr << "Failed to open " << img_file
					<< " or " << lbl_file << "\n";
				return false;
			}

			images.clear();
			labels.clear();

			// reserve expected size (MNIST known counts)
			if (split == "train") {
				images.reserve(TRAIN_SIZE);
				labels.reserve(TRAIN_SIZE);
			}
			else if (split == "test") {
				images.reserve(TEST_SIZE);
				labels.reserve(TEST_SIZE);
			}

			std::array<uint8_t, IMAGE_SIZE> buffer{};
			uint8_t label;

			size_t count = 0;
			while (
				img_in.read(reinterpret_cast<char*>(buffer.data()), IMAGE_SIZE) &&
				lbl_in.read(reinterpret_cast<char*>(&label), 1)
				) {
				std::vector<float> img{};
				img.resize(IMAGE_SIZE);
				for (size_t i = 0; i < IMAGE_SIZE; ++i) {
					img[i] = buffer[i] / 255.0f;
				}

				images.push_back(img);
				labels.push_back(label);
				++count;
			}

			std::cout << "Loaded " << count << " samples from " << split << "\n";
			return true;
		}

		size_t size() const { return images.size(); }
		const std::vector<float>& get_image(size_t idx) const { return images[idx]; }
		uint8_t get_label(size_t idx) const { return labels[idx]; }

	private:
		std::string path;
		std::vector<std::vector<float>> images;
		std::vector<uint8_t> labels;
	};

} // namespace Heron
