#include "nodes.h"

#include <Heron/Activations.h>
#include <ImNodeFlow.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>

namespace Nodes {
HiddenLayerNode::HiddenLayerNode() {
  setTitle("Hidden Layer");

  setStyle([] {
    ImFlow::NodeStyle s = *ImFlow::NodeStyle::cyan();
    s.header_bg = IM_COL32(71, 142, 173, 255);
    s.radius = 10.f;
    return std::make_shared<ImFlow::NodeStyle>(s);
  }());

  // ----------------------------
  // INPUT: PREVIOUS MODEL
  // ----------------------------
  addIN<Model>("Previous Layer", Model{}, ImFlow::ConnectionFilter::SameType(),
               [] {
                 auto s = *ImFlow::PinStyle::blue();
                 s.socket_shape = 0;
                 s.color = IM_COL32(39, 84, 138, 255);
                 return std::make_shared<ImFlow::PinStyle>(s);
               }());

  // ----------------------------
  // INPUT: NEURON COUNT
  // ----------------------------
  addIN<int>("Neurons", 0, ImFlow::ConnectionFilter::SameType(), [] {
    auto s = *ImFlow::PinStyle::red();
    s.socket_shape = 4;
    s.color = IM_COL32(184, 92, 56, 255);
    return std::make_shared<ImFlow::PinStyle>(s);
  }());

  // ----------------------------
  // INPUT: ACTIVATION
  // ----------------------------
  addIN<ActivationEntry>("Activation", Activations[0],
                         ImFlow::ConnectionFilter::SameType(), [] {
                           auto s = *ImFlow::PinStyle::red();
                           s.socket_shape = 3;
                           s.color = IM_COL32(191, 90, 90, 255);
                           return std::make_shared<ImFlow::PinStyle>(s);
                         }());

  // ----------------------------
  // OUTPUT: MODEL
  // ----------------------------
  addOUT<Model>("",
                [] {
                  auto s = *ImFlow::PinStyle::blue();
                  s.socket_shape = 0;
                  s.color = IM_COL32(39, 84, 138, 255);
                  return std::make_shared<ImFlow::PinStyle>(s);
                }())
      ->behaviour([this]() { return model; });
}

void HiddenLayerNode::draw() {
  // ----------------------------
  // PULL INPUT VALUES
  // ----------------------------
  neurons = getInVal<int>("Neurons");
  if (neurons < 0) neurons = 0;

  model = getInVal<Model>("Previous Layer");

  if (neurons > 0) model.layers.push_back(static_cast<size_t>(neurons));

  model.activations.push_back(getInVal<ActivationEntry>("Activation"));
}

// ----------------------------
// SERIALIZATION
// ----------------------------
nlohmann::json HiddenLayerNode::save() { return {{"neurons", neurons}}; }

void HiddenLayerNode::load(const nlohmann::json& j) {
  if (j.contains("neurons")) neurons = j["neurons"].get<int>();

  if (neurons < 1) neurons = 1;
}
}  // namespace Nodes