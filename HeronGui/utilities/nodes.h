#pragma once
#include <Heron/Activations.h>
#include <ImNodeFlow.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <vector>

namespace Nodes {
struct ActivationEntry {
  ActFn fn;
  ActFn deriv;
  const char* name;
};

static const ActivationEntry Activations[] = {
    {nullptr, nullptr, "None"},
    {Heron::Activation::relu, Heron::Activation::relu_deriv, "ReLU"},
    //{Heron::Activation::tanh, Heron::Activation::tanh_deriv, "TANH"},
    {Heron::Activation::softmax, nullptr, "Softmax"}, // update index in OutputLayerNode
};

struct Model {
  std::vector<size_t> layers;
  std::vector<ActivationEntry> activations;
};

struct InputLayerNode : public ImFlow::BaseNode {
  int neurons = 28 * 28;
  Model model;

  InputLayerNode() {
    setTitle("Input Layer");
    setStyle([] {
      ImFlow::NodeStyle s = *ImFlow::NodeStyle::green();
      s.header_bg = IM_COL32(39, 84, 138, 255);
      s.radius = 10.f;
      return std::make_shared<ImFlow::NodeStyle>(s);
    }());

    addOUT<Model>("",
                  [] {
                    auto s = *ImFlow::PinStyle::blue();
                    s.socket_shape = 0;
                    s.color = IM_COL32(39, 84, 138, 255);
                    return std::make_shared<ImFlow::PinStyle>(s);
                  }())
        ->behaviour([this]() { return model; });
  }

  void draw() override {
    model.layers.clear();
    model.activations.clear();

    if (neurons > 0) model.layers.push_back(static_cast<size_t>(neurons));

    model.activations.push_back(Activations[0]);

    ImGui::BeginDisabled();

    ImGui::SetNextItemWidth(100.f);
    ImGui::DragInt("##neurons", &neurons);

    ImGui::SetNextItemWidth(100.f);
    int selected = 0;  // None
    const char* preview = Activations[selected].name;
    if (ImGui::BeginCombo("##activation_combo", preview)) {
      for (int i = 0; i < IM_ARRAYSIZE(Activations); i++) {
        bool isSelected = (selected == i);
        if (ImGui::Selectable(Activations[i].name, isSelected)) selected = i;

        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    ImGui::EndDisabled();
  }
};

struct OutputLayerNode : public ImFlow::BaseNode {
  Model model;
  int neurons = 10;

  OutputLayerNode() {
    setTitle("Output Layer");
    setStyle([] {
      ImFlow::NodeStyle s = *ImFlow::NodeStyle::green();
      s.header_bg = IM_COL32(39, 84, 138, 255);
      s.radius = 10.f;
      return std::make_shared<ImFlow::NodeStyle>(s);
    }());

    addIN<Model>("", Model{}, ImFlow::ConnectionFilter::SameType(), [] {
      auto s = *ImFlow::PinStyle::blue();
      s.socket_shape = 0;
      s.color = IM_COL32(39, 84, 138, 255);
      return std::make_shared<ImFlow::PinStyle>(s);
    }());
  }

  void draw() override {
    model.layers.clear();
    model.activations.clear();

    model = getInVal<Model>("");

    if (neurons > 0) model.layers.push_back(static_cast<size_t>(neurons));

    model.activations.push_back(Activations[2]); // softmax

    ImGui::BeginDisabled();

    ImGui::SetNextItemWidth(100.f);
    ImGui::DragInt("##neurons", &neurons);

    ImGui::SetNextItemWidth(100.f);
    int selected = 2;  // Softmax
    const char* preview = Activations[selected].name;
    if (ImGui::BeginCombo("##activation_combo", preview)) {
      for (int i = 0; i < IM_ARRAYSIZE(Activations); i++) {
        bool isSelected = (selected == i);
        if (ImGui::Selectable(Activations[i].name, isSelected)) selected = i;

        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
    ImGui::EndDisabled();
  }
};

struct HiddenLayerNode : public ImFlow::BaseNode {
  Model model;
  int neurons = 0;

  HiddenLayerNode() {
    setTitle("Hidden Layer");
    setStyle([] {
      ImFlow::NodeStyle s = *ImFlow::NodeStyle::cyan();
      s.header_bg = IM_COL32(71, 142, 173, 255);
      s.radius = 10.f;
      return std::make_shared<ImFlow::NodeStyle>(s);
    }());

    addIN<Model>("Previous Layer", Model{},
                 ImFlow::ConnectionFilter::SameType(), [] {
                   auto s = *ImFlow::PinStyle::blue();
                   s.socket_shape = 0;
                   s.color = IM_COL32(39, 84, 138, 255);
                   return std::make_shared<ImFlow::PinStyle>(s);
                 }());

    addIN<int>("Neurons", 0, ImFlow::ConnectionFilter::SameType(), [] {
      auto s = *ImFlow::PinStyle::red();
      s.socket_shape = 4;
      s.color = IM_COL32(184, 92, 56, 255);
      return std::make_shared<ImFlow::PinStyle>(s);
    }());

    addIN<ActivationEntry>("Activation", Activations[0],
                           ImFlow::ConnectionFilter::SameType(), [] {
                             auto s = *ImFlow::PinStyle::red();
                             s.socket_shape = 3;
                             s.color = IM_COL32(191, 90, 90, 255);
                             return std::make_shared<ImFlow::PinStyle>(s);
                           }());

    addOUT<Model>("",
                  [] {
                    auto s = *ImFlow::PinStyle::blue();
                    s.socket_shape = 0;
                    s.color = IM_COL32(39, 84, 138, 255);
                    return std::make_shared<ImFlow::PinStyle>(s);
                  }())
        ->behaviour([this]() { return model; });
  }

  void draw() override {
    neurons = getInVal<int>("Neurons");

    model.layers.clear();
    model.activations.clear();

    model = getInVal<Model>("Previous Layer");

    if (neurons > 0) model.layers.push_back(static_cast<size_t>(neurons));

    model.activations.push_back(getInVal<ActivationEntry>("Activation"));
  }
};

struct ActivationNode : public ImFlow::BaseNode {
  int selected = 0;  // index into Activations

  ActivationNode() {
    setTitle("Activation");
    setStyle(ImFlow::NodeStyle::red());

    addOUT<ActivationEntry>("",
                            [] {
                              auto s = *ImFlow::PinStyle::red();
                              s.socket_shape = 3;
                              s.color = IM_COL32(191, 90, 90, 255);
                              return std::make_shared<ImFlow::PinStyle>(s);
                            }())
        ->behaviour([this]() { return Activations[selected]; });
  }

  void draw() override {
    ImGui::SetNextItemWidth(100.f);

    const char* preview = Activations[selected].name;
    if (ImGui::BeginCombo("##activation_combo", preview)) {
      for (int i = 0; i < IM_ARRAYSIZE(Activations); i++) {
        bool isSelected = (selected == i);
        if (ImGui::Selectable(Activations[i].name, isSelected)) selected = i;

        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }
  }
};

struct NeuronsNode : public ImFlow::BaseNode {
  int neurons = 0;

  NeuronsNode() {
    setTitle("Neurons");
    setStyle([] {
      ImFlow::NodeStyle s = *ImFlow::NodeStyle::green();
      s.header_bg = IM_COL32(184, 92, 56, 255);
      s.radius = 10.f;
      return std::make_shared<ImFlow::NodeStyle>(s);
    }());

    addOUT<int>("",
                [] {
                  auto s = *ImFlow::PinStyle::red();
                  s.socket_shape = 4;
                  s.color = IM_COL32(184, 92, 56, 255);
                  return std::make_shared<ImFlow::PinStyle>(s);
                }())
        ->behaviour([this]() { return neurons; });
  }

  void draw() override {
    ImGui::SetNextItemWidth(70.f);
    ImGui::DragInt("##neurons", &neurons);
  }
};

struct NodeBinary {
  int id;
  int type;  // 0 = Input, 1 = Hidden, 2 = Output, 3 = Activation, 4 = Neurons
  float posX;
  float posY;

  // Node-specific data
  int neurons;         // for HiddenLayerNode or NeuronsNode
  int activationType;  // if you encode activation functions as ints
};

struct LinkBinary {
  int fromId;
  int toId;
};

struct NodeEditor : ImFlow::BaseNode {
  ImFlow::ImNodeFlow mINF;
  ImFlow::BaseNode* contextNode = nullptr;
  ImVec2 openPopupMousePos{};
  ImVec2 mSize{};
  Model mModel;

  NodeEditor(float size) : ImFlow::BaseNode() { clear(); }

  void set_size(ImVec2 size) {
    mINF.setSize(size);
    mSize = size;
  }

  void draw() override {
    mINF.update();
    handleContextMenus();
    updateModelFromGraph();
  }

  const Model* getModel() {
    const auto& nodes = mINF.getNodes();

    for (const auto& [uid, node] : nodes) {
      if (auto* out = dynamic_cast<OutputLayerNode*>(node.get())) {
        return &out->model;
      }
    }
    return nullptr;
  }

 private:
  void handleContextMenus() {
    // Check if mouse is over the last ImGui item (the node editor canvas)
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
      openPopupMousePos = ImGui::GetMousePos();
      ImGui::OpenPopup("Background Context Menu");
    }

    drawBackgroundContextMenu();
  }

  void drawBackgroundContextMenu() {
    if (ImGui::BeginPopup("Background Context Menu", ImGuiWindowFlags_NoMove)) {
      if (ImGui::MenuItem("Add Input Layer"))
        mINF.addNode<InputLayerNode>(mINF.screen2grid(openPopupMousePos));

      if (ImGui::MenuItem("Add Hidden Layer"))
        mINF.addNode<HiddenLayerNode>(mINF.screen2grid(openPopupMousePos));

      if (ImGui::MenuItem("Add Output Layer"))
        mINF.addNode<OutputLayerNode>(mINF.screen2grid(openPopupMousePos));

      if (ImGui::MenuItem("Add Activation Functions"))
        mINF.addNode<ActivationNode>(mINF.screen2grid(openPopupMousePos));

      if (ImGui::MenuItem("Add Neurons"))
        mINF.addNode<NeuronsNode>(mINF.screen2grid(openPopupMousePos));

      ImGui::Separator();

      if (ImGui::MenuItem("Clear All")) clear();

      ImGui::EndPopup();
    }
  }

  void linkDropped(ImFlow::Pin* droppedFrom) {
    if (!droppedFrom) return;

    if (!ImGui::IsPopupOpen("Link To Pin")) ImGui::OpenPopup("Link To Pin");

    if (ImGui::BeginPopup("Link To Pin", ImGuiWindowFlags_NoMove)) {
      if (ImGui::MenuItem("Input Layer"))
        mINF.addNode<InputLayerNode>(mINF.screen2grid(openPopupMousePos));

      if (ImGui::MenuItem("Hidden Layer"))
        mINF.addNode<HiddenLayerNode>(mINF.screen2grid(openPopupMousePos));

      if (ImGui::MenuItem("Output Layer"))
        mINF.addNode<OutputLayerNode>(mINF.screen2grid(openPopupMousePos));

      if (ImGui::MenuItem("Activation Functions"))
        mINF.addNode<ActivationNode>(mINF.screen2grid(openPopupMousePos));

      if (ImGui::MenuItem("Neurons"))
        mINF.addNode<NeuronsNode>(mINF.screen2grid(openPopupMousePos));

      ImGui::Separator();

      if (ImGui::MenuItem("Clear All")) clear();

      ImGui::EndPopup();
    }
  }

  void clear() {
    // Clear all nodes and links
    mINF = ImFlow::ImNodeFlow();
    mINF.setSize(mSize);

    // Setup the "link dropped" popup
    mINF.droppedLinkPopUpContent(
        [this](ImFlow::Pin* fromPin) { linkDropped(fromPin); });
  }

  void updateModelFromGraph() {
    mModel = {};  // reset

    const auto& nodes = mINF.getNodes();

    for (const auto& [uid, node] : nodes) {
      if (auto* out = dynamic_cast<OutputLayerNode*>(node.get())) {
        mModel = out->model;
        break;
      }
    }
  }
};
}  // namespace Nodes