#include "Heron/Network.h"

#include <cstring>
#include <fstream>
#include <stdexcept>

#include "Heron/Utils.h"

namespace Heron {
static const ActivationEntry ACT_TABLE[] = {
    {Activation::relu, Activation::relu_deriv, "relu"},
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
  Z.resize(L);
  A.resize(L + 1);

  dW.resize(L);
  db.resize(L);

  activation_derivs.reserve(activations.size());
  for (ActFn fn : activations)
    activation_derivs.push_back(find_by_fn(fn).deriv);

  if (initialize) init_params();
}

void Network::init_params() {
  for (size_t l = 0; l < weights.size(); l++) {
    const size_t in = layer_sizes[l];
    const size_t out = layer_sizes[l + 1];

    weights[l].assign(out, std::vector<float>(in));
    biases[l].assign(out, 0.0f);
    dW[l].assign(out, std::vector<float>(in));
    db[l].assign(out, 0.0f);

    for (size_t i = 0; i < out; i++)
      for (size_t j = 0; j < in; j++)
        weights[l][i][j] =
            (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
  }
}

void Network::forward(const std::vector<float>& input) {
  // copy input to the first layer
  A[0] = input;

  // compute forward pass into class members
  for (size_t l = 0; l < weights.size(); l++) {
    Z[l] = Utils::mat_vec(weights[l], A[l], biases[l]);
    A[l + 1] = activation_fns[l](Z[l]);
  }
}

void Network::backward(int Y) {
  const size_t L = weights.size();

  std::vector<std::vector<float>> dZ(L);
  std::vector<std::vector<float>> dA(L);

  /* ---- output layer ---- */

  const size_t C = A[L].size();
  auto Y_oh = Utils::one_hot(Y, static_cast<int>(C));

  dZ[L - 1].resize(C);
  for (size_t i = 0; i < C; i++) dZ[L - 1][i] = A[L][i] - Y_oh[i];

  /* ---- backprop ---- */

  for (int l = (int)L - 1; l >= 0; l--) {
    Utils::outer_product(dZ[l], A[l], dW[l]);
    db[l] = dZ[l];

    if (l == 0) continue;

    dA[l - 1] = Utils::matT_vec(weights[l], dZ[l]);

    if (activation_derivs[l - 1]) {
      dZ[l - 1] = activation_derivs[l - 1](Z[l - 1]);

      for (size_t i = 0; i < dZ[l - 1].size(); i++)
        dZ[l - 1][i] *= dA[l - 1][i];
    } else {
      // softmax + cross entropy shortcut
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

  // resize output buffers
  A_out.resize(L + 1);
  Z_out.resize(L);

  // input layer
  A_out[0] = input;

  // forward pass
  for (size_t l = 0; l < L; l++) {
    Z_out[l] = Utils::mat_vec(weights[l], A_out[l], biases[l]);
    A_out[l + 1] = activation_fns[l](Z_out[l]);
  }
}

}  // namespace Heron
