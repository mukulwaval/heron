#include "Heron/FileIO.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace Heron {
std::vector<float> FileIO::read_28x28_bmp(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("Couldn't open BMP: " + path);

  BMPFileHeader fileHeader;
  BMPInfoHeader infoHeader;
  in.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
  in.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

  if (fileHeader.bfType != 0x4D42) throw std::runtime_error("Not a BMP file");
  if (infoHeader.biWidth != 28 || infoHeader.biHeight != 28)
    throw std::runtime_error("BMP ain't 28x28");
  if (infoHeader.biBitCount != 24 || infoHeader.biCompression != 0)
    throw std::runtime_error("Only 24-bit uncompressed BMP supported");

  int rowSize = ((infoHeader.biBitCount * infoHeader.biWidth + 31) / 32) * 4;
  std::vector<unsigned char> row(rowSize);

  std::vector<float> img(28 * 28);
  in.seekg(fileHeader.bfOffBits, std::ios::beg);

  for (int y = 27; y >= 0; --y) {
    in.read(reinterpret_cast<char*>(row.data()), rowSize);
    for (int x = 0; x < 28; ++x) {
      unsigned char b = row[x * 3 + 0];
      unsigned char g = row[x * 3 + 1];
      unsigned char r = row[x * 3 + 2];
      img[y * 28 + x] = r / 255.0f;
    }
  }
  return img;
}

void FileIO::write_28x28_bmp(const std::vector<float>& img,
                             const std::string& path) {
  if (img.size() != 28 * 28)
    throw std::runtime_error("Image buffer ain't 28x28");

  std::filesystem::create_directories(
      std::filesystem::path(path).parent_path());

  std::ofstream out(path, std::ios::binary);
  if (!out) throw std::runtime_error("Couldn't write BMP: " + path);

  int rowSize = ((24 * 28 + 31) / 32) * 4;
  int imageSize = rowSize * 28;

  BMPFileHeader fileHeader{};
  BMPInfoHeader infoHeader{};

  fileHeader.bfType = 0x4D42;
  fileHeader.bfOffBits = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
  fileHeader.bfSize = fileHeader.bfOffBits + imageSize;

  infoHeader.biSize = sizeof(BMPInfoHeader);
  infoHeader.biWidth = 28;
  infoHeader.biHeight = 28;
  infoHeader.biPlanes = 1;
  infoHeader.biBitCount = 24;
  infoHeader.biCompression = 0;
  infoHeader.biSizeImage = imageSize;

  out.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
  out.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

  std::vector<unsigned char> row(rowSize, 0);

  for (int y = 27; y >= 0; --y) {
    for (int x = 0; x < 28; ++x) {
      float v = img[y * 28 + x];
      v = std::clamp(v, 0.0f, 1.0f);
      unsigned char px = static_cast<unsigned char>(v * 255.0f);
      row[x * 3 + 0] = px;
      row[x * 3 + 1] = px;
      row[x * 3 + 2] = px;
    }
    out.write(reinterpret_cast<char*>(row.data()), rowSize);
  }
}
}  // namespace Heron