#include "Heron/Network.h"

#include <cstring>
#include <fstream>
#include <random>
#include <stdexcept>

#include "Heron/Utils.h"

namespace Heron {
static const ActivationEntry ACT_TABLE[] = {
    {Activation::relu, Activation::relu_deriv, "relu"},
    {Activation::tanh, Activation::tanh_deriv, "tanh"},
    {Activation::softmax, nullptr, "softmax"},
};

static constexpr size_t ACT_COUNT = sizeof(ACT_TABLE) / sizeof(ACT_TABLE[0]);

static const ActivationEntry& find_by_fn(ActFn fn) {
  const char* fn_type = typeid(fn).name();

  for (size_t i = 0; i < ACT_COUNT; i++) {
    if (std::strcmp(typeid(ACT_TABLE[i].fn).name(), fn_type) == 0)
      return ACT_TABLE[i];
  }

  throw std::runtime_error("Unknown activation function");
}

static const ActivationEntry& find_by_name(const std::string& name) {
  for (size_t i = 0; i < ACT_COUNT; i++)
    if (name == ACT_TABLE[i].name) return ACT_TABLE[i];

  throw std::runtime_error("Unknown activation name");
}

Network::Network(const std::vector<size_t>& layer_sizes,
                 const std::vector<ActFn>& activations, bool initialize)
    : layer_sizes(layer_sizes), activation_fns(activations) {
  const size_t L = layer_sizes.size() - 1;

  weights.resize(L);
  biases.resize(L);
  dW.resize(L);
  db.resize(L);

  Z.resize(L);
  A.resize(L + 1);

  activation_derivs.resize(activations.size());
  for (size_t i = 0; i < activations.size(); i++)
    activation_derivs[i] = find_by_fn(activations[i]).deriv;

  if (initialize) init_params();
}

void Network::init_params() {
  std::mt19937 rng(std::random_device{}());
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

  for (size_t l = 0; l < weights.size(); l++) {
    size_t in = layer_sizes[l], out = layer_sizes[l + 1];
    weights[l].resize(out);
    dW[l].resize(out);

    for (size_t i = 0; i < out; i++) {
      weights[l][i].resize(in);
      dW[l][i].resize(in);
      for (size_t j = 0; j < in; j++) weights[l][i][j] = dist(rng);
    }

    biases[l].resize(out);
    db[l].resize(out);
  }
}

void Network::forward(const std::vector<float>& input) {
  A[0].assign(input.begin(), input.end());

  for (size_t l = 0; l < weights.size(); l++) {
    // reuse Z[l] to avoid allocation
    Utils::mat_vec_inplace(weights[l], A[l], biases[l], Z[l]);
    A[l + 1] = activation_fns[l](Z[l]);
  }
}

void Network::backward(int Y) {
  const size_t L = weights.size();

  std::vector<std::vector<float>> dZ(L);
  std::vector<std::vector<float>> dA(L);

  const size_t C = A[L].size();
  auto Y_oh = Utils::one_hot(Y, static_cast<int>(C));

  if (dZ[L - 1].size() != C) dZ[L - 1].resize(C);
  for (size_t i = 0; i < C; i++) dZ[L - 1][i] = A[L][i] - Y_oh[i];

  for (int l = (int)L - 1; l >= 0; l--) {
    Utils::outer_product_inplace(dZ[l], A[l], dW[l]);
    db[l] = dZ[l];

    if (l == 0) continue;

    if (dA[l - 1].size() != layer_sizes[l]) dA[l - 1].resize(layer_sizes[l]);
    Utils::matT_vec_inplace(weights[l], dZ[l], dA[l - 1]);

    if (activation_derivs[l - 1]) {
      auto deriv = activation_derivs[l - 1](Z[l - 1]);

      if (dZ[l - 1].size() != deriv.size()) dZ[l - 1].resize(deriv.size());

      for (size_t i = 0; i < deriv.size(); i++)
        dZ[l - 1][i] = deriv[i] * dA[l - 1][i];
    } else {
      dZ[l - 1] = dA[l - 1];
    }
  }
}

void Network::update(float lr) {
  for (size_t l = 0; l < weights.size(); l++) {
    for (size_t i = 0; i < weights[l].size(); i++)
      for (size_t j = 0; j < weights[l][i].size(); j++)
        weights[l][i][j] -= lr * dW[l][i][j];

    for (size_t i = 0; i < biases[l].size(); i++) biases[l][i] -= lr * db[l][i];
  }
}

const std::vector<float>& Network::output() const { return A.back(); }

void Network::save_model(const std::string& path) const {
  std::ofstream out(path, std::ios::binary);
  if (!out) throw std::runtime_error("save_model failed");

  size_t layers = layer_sizes.size();
  out.write((char*)&layers, sizeof(layers));
  out.write((char*)layer_sizes.data(), layers * sizeof(size_t));

  for (ActFn fn : activation_fns) {
    const char* name = find_by_fn(fn).name;
    size_t len = std::strlen(name);
    out.write((char*)&len, sizeof(len));
    out.write(name, len);
  }

  for (auto& W : weights) {
    size_t r = W.size(), c = W[0].size();
    out.write((char*)&r, sizeof(r));
    out.write((char*)&c, sizeof(c));
    for (auto& row : W) out.write((char*)row.data(), c * sizeof(float));
  }

  for (auto& b : biases) {
    size_t n = b.size();
    out.write((char*)&n, sizeof(n));
    out.write((char*)b.data(), n * sizeof(float));
  }
}

void Network::load_model(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("load_model failed");

  size_t layers;
  in.read((char*)&layers, sizeof(layers));

  layer_sizes.resize(layers);
  in.read((char*)layer_sizes.data(), layers * sizeof(size_t));

  activation_fns.clear();
  activation_derivs.clear();

  for (size_t i = 1; i < layers; i++) {
    size_t len;
    in.read((char*)&len, sizeof(len));
    std::string name(len, '\0');
    in.read(name.data(), len);

    const auto& e = find_by_name(name);
    activation_fns.push_back(e.fn);
    activation_derivs.push_back(e.deriv);
  }

  const size_t L = layers - 1;
  weights.resize(L);
  biases.resize(L);
  dW.resize(L);
  db.resize(L);

  for (size_t l = 0; l < L; l++) {
    size_t r, c;
    in.read((char*)&r, sizeof(r));
    in.read((char*)&c, sizeof(c));

    weights[l].assign(r, std::vector<float>(c));
    dW[l].assign(r, std::vector<float>(c));

    for (size_t i = 0; i < r; i++)
      in.read((char*)weights[l][i].data(), c * sizeof(float));
  }

  for (size_t l = 0; l < L; l++) {
    size_t n;
    in.read((char*)&n, sizeof(n));
    biases[l].resize(n);
    db[l].resize(n);
    in.read((char*)biases[l].data(), n * sizeof(float));
  }

  Z.assign(L, {});
  A.assign(L + 1, {});
}

void Network::predict(const std::vector<float>& input,
                      std::vector<std::vector<float>>& Z_out,
                      std::vector<std::vector<float>>& A_out) const {
  const size_t L = weights.size();

  Z_out.resize(L);
  A_out.resize(L + 1);
  A_out[0] = input;  // copy input once

  for (size_t l = 0; l < L; l++) {
    Utils::mat_vec_inplace(weights[l], A_out[l], biases[l], Z_out[l]);
    if (A_out[l + 1].size() != Z_out[l].size())
      A_out[l + 1].resize(Z_out[l].size());
    A_out[l + 1] = activation_fns[l](Z_out[l]);
  }
}

size_t Network::layer_count() const { return layer_sizes.size(); }
}  // namespace Heron
