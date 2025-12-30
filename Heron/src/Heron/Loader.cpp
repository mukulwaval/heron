#include "Heron/Loader.h"

#include <cstdint>
#include <fstream>
#include <iostream>

constexpr auto TRAIN_SIZE = 60000;
constexpr auto TEST_SIZE = 10000;
constexpr int WIDTH = 28;
constexpr int HEIGHT = 28;
constexpr int IMAGE_SIZE = WIDTH * HEIGHT;

namespace Heron {
Loader::Loader(const std::string& dataset_path) : path(dataset_path) {}

Loader::~Loader() = default;

bool Loader::load(const std::string& split) {
  std::string img_file = path + "/" + split + "_images.hrndt";
  std::string lbl_file = path + "/" + split + "_labels.hrndt";

  std::ifstream img_in(img_file, std::ios::binary);
  std::ifstream lbl_in(lbl_file, std::ios::binary);

  if (!img_in || !lbl_in) {
    std::cerr << "Failed to open " << img_file << " or " << lbl_file << "\n";
    return false;
  }

  images.clear();
  labels.clear();

  // reserve expected size (MNIST known counts)
  if (split == "train") {
    images.reserve(TRAIN_SIZE);
    labels.reserve(TRAIN_SIZE);
  } else if (split == "test") {
    images.reserve(TEST_SIZE);
    labels.reserve(TEST_SIZE);
  }

  std::array<uint8_t, IMAGE_SIZE> buffer{};
  uint8_t label;

  size_t count = 0;

  constexpr float INV_255 = 1.0f / 255.0f;

  while (img_in.read(reinterpret_cast<char*>(buffer.data()), IMAGE_SIZE) &&
         lbl_in.read(reinterpret_cast<char*>(&label), 1)) {
    std::vector<float> img(IMAGE_SIZE);

    for (size_t i = 0; i < IMAGE_SIZE; ++i) img[i] = buffer[i] * INV_255;

    images.push_back(std::move(img));
    labels.push_back(label);
    ++count;
  }

  std::cout << "Loaded " << count << " samples from " << split << "\n";
  return true;
}

size_t Loader::size() const { return images.size(); }

const std::vector<float>& Loader::get_image(size_t idx) const {
  return images[idx];
}

uint8_t Loader::get_label(size_t idx) const { return labels[idx]; }
}  // namespace Heron