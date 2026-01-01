#pragma once
#include <Heron/Activations.h>
#include <ImNodeFlow.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <vector>

namespace Nodes {
struct ISerializableNode {
  virtual ~ISerializableNode() = default;
  virtual nlohmann::json save() = 0;
  virtual void load(const nlohmann::json&) = 0;
};

struct ActivationEntry {
  ActFn fn;
  ActFn deriv;
  const char* name;
};

static const ActivationEntry Activations[] = {
    {nullptr, nullptr, "None"},
    {Heron::Activation::relu, Heron::Activation::relu_deriv, "ReLU"},
    //{Heron::Activation::tanh, Heron::Activation::tanh_deriv, "TANH"},
    {Heron::Activation::softmax, nullptr,
     "Softmax"},  // update index in OutputLayerNode
};

struct Model {
  std::vector<size_t> layers;
  std::vector<ActivationEntry> activations;
};

struct InputLayerNode final : public ImFlow::BaseNode,
                              public ISerializableNode {
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

    // Input has no activation
    model.activations.push_back(Activations[0]);

    ImGui::BeginDisabled();

    ImGui::SetNextItemWidth(100.f);
    ImGui::DragInt("##neurons", &neurons);

    ImGui::SetNextItemWidth(100.f);
    const char* preview = Activations[0].name;
    if (ImGui::BeginCombo("##activation_combo", preview)) {
      for (int i = 0; i < IM_ARRAYSIZE(Activations); i++) {
        bool isSelected = (i == 0);
        ImGui::Selectable(Activations[i].name, isSelected);
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    ImGui::EndDisabled();
  }

  // --------------------
  // SERIALIZATION
  // --------------------

  nlohmann::json save() override { return {{"neurons", neurons}}; }

  void load(const nlohmann::json& j) override {
    if (j.contains("neurons")) neurons = j["neurons"].get<int>();

    if (neurons < 1) neurons = 1;
  }
};

struct OutputLayerNode final : public ImFlow::BaseNode,
                               public ISerializableNode {
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
    model = getInVal<Model>("");

    if (neurons > 0) model.layers.push_back(static_cast<size_t>(neurons));

    // hard-coded softmax
    model.activations.push_back(Activations[2]);

    // UI is locked on purpose
    ImGui::BeginDisabled();

    ImGui::SetNextItemWidth(100.f);
    ImGui::DragInt("##neurons", &neurons);

    ImGui::SetNextItemWidth(100.f);
    const char* preview = Activations[2].name;
    if (ImGui::BeginCombo("##activation_combo", preview)) {
      for (int i = 0; i < IM_ARRAYSIZE(Activations); i++) {
        bool isSelected = (i == 2);
        ImGui::Selectable(Activations[i].name, isSelected);
        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    ImGui::EndDisabled();
  }

  // --------------------
  // SERIALIZATION
  // --------------------

  nlohmann::json save() override { return {{"neurons", neurons}}; }

  void load(const nlohmann::json& j) override {
    if (j.contains("neurons")) neurons = j["neurons"].get<int>();

    if (neurons < 1) neurons = 1;
  }
};

struct HiddenLayerNode final : public ImFlow::BaseNode,
                               public ISerializableNode {
  Model model;
  int neurons = 1;

  HiddenLayerNode();
  void draw() override;
  nlohmann::json save() override;
  void load(const nlohmann::json& j) override;
};

struct ActivationNode final : public ImFlow::BaseNode,
                              public ISerializableNode {
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
        ->behaviour([this]() {
          // clamp just in case
          if (selected < 0) selected = 0;
          if (selected >= IM_ARRAYSIZE(Activations))
            selected = IM_ARRAYSIZE(Activations) - 1;

          return Activations[selected];
        });
  }

  void draw() override {
    ImGui::SetNextItemWidth(100.f);

    if (selected < 0) selected = 0;
    if (selected >= IM_ARRAYSIZE(Activations))
      selected = IM_ARRAYSIZE(Activations) - 1;

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

  // --------------------
  // SERIALIZATION
  // --------------------

  nlohmann::json save() override { return {{"selected", selected}}; }

  void load(const nlohmann::json& j) override {
    if (j.contains("selected")) selected = j["selected"].get<int>();

    // safety clamp
    if (selected < 0) selected = 0;
    if (selected >= IM_ARRAYSIZE(Activations))
      selected = IM_ARRAYSIZE(Activations) - 1;
  }
};

struct NeuronsNode final : public ImFlow::BaseNode, public ISerializableNode {
  int neurons = 1;

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
    if (neurons < 1) neurons = 1;
    ImGui::SetNextItemWidth(70.f);
    ImGui::DragInt("##neurons", &neurons, 1, 1, 4096);
  }

  // --------------------
  // SERIALIZATION
  // --------------------

  nlohmann::json save() override { return {{"neurons", neurons}}; }

  void load(const nlohmann::json& j) override {
    if (j.contains("neurons")) neurons = j["neurons"].get<int>();

    if (neurons < 1) neurons = 1;
  }
};

struct SavedNode {
  ImFlow::NodeUID uid;
  std::string type;
  ImVec2 pos;
  nlohmann::json data;  // per-node custom shit
};

struct SavedLink {
  ImFlow::NodeUID fromNode;
  ImFlow::PinUID fromPin;
  ImFlow::NodeUID toNode;
  ImFlow::PinUID toPin;
};

struct SavedGraph {
  std::vector<SavedNode> nodes;
  std::vector<SavedLink> links;
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

  static SavedGraph saveGraph(ImFlow::ImNodeFlow& flow) {
    SavedGraph out;

    // -------- SAVE NODES --------
    for (auto& [uid, node] : flow.getNodes()) {
      SavedNode sn;
      sn.uid = uid;
      sn.type = node->getName();  // or custom nodeType string
      sn.pos = node->getPos();

      // Let each node serialize itself
      if (auto* serial = dynamic_cast<ISerializableNode*>(node.get()))
        sn.data = serial->save();

      out.nodes.push_back(sn);
    }

    // -------- SAVE LINKS --------
    for (auto& weak : flow.getLinks()) {
      if (auto link = weak.lock()) {
        SavedLink sl;
        sl.fromNode = link->left()->getParent()->getUID();
        sl.fromPin = link->left()->getUid();
        sl.toNode = link->right()->getParent()->getUID();
        sl.toPin = link->right()->getUid();
        out.links.push_back(sl);
      }
    }

    return out;
  }

  void loadGraph(const SavedGraph& graph) {
    // ----------------------------
    // 1. NUKE OLD GRAPH
    // ----------------------------
    clear();

    std::unordered_map<ImFlow::NodeUID, ImFlow::BaseNode*> uidToNode;

    // ----------------------------
    // 2. RECREATE NODES
    // ----------------------------
    for (const SavedNode& sn : graph.nodes) {
      std::shared_ptr<ImFlow::BaseNode> node;

      if (sn.type == "Input Layer")
        node = mINF.addNode<InputLayerNode>(sn.pos);
      else if (sn.type == "Hidden Layer")
        node = mINF.addNode<HiddenLayerNode>(sn.pos);
      else if (sn.type == "Output Layer")
        node = mINF.addNode<OutputLayerNode>(sn.pos);
      else if (sn.type == "Activation")
        node = mINF.addNode<ActivationNode>(sn.pos);
      else if (sn.type == "Neurons")
        node = mINF.addNode<NeuronsNode>(sn.pos);
      else
        continue;  // unknown node type

      // restore UID
      node->setUID(sn.uid);

      // restore custom data
      if (auto* serial = dynamic_cast<ISerializableNode*>(node.get()))
        serial->load(sn.data);

      uidToNode[sn.uid] = node.get();
    }

    // ----------------------------
    // 3. RECREATE LINKS
    // ----------------------------
    for (const SavedLink& sl : graph.links) {
      auto itFrom = uidToNode.find(sl.fromNode);
      auto itTo = uidToNode.find(sl.toNode);

      if (itFrom == uidToNode.end() || itTo == uidToNode.end()) continue;

      ImFlow::BaseNode* fromNode = itFrom->second;
      ImFlow::BaseNode* toNode = itTo->second;

      ImFlow::Pin* outPin = nullptr;
      for (auto& p : fromNode->getOuts()) {
        if (p->getUid() == sl.fromPin) {
          outPin = p.get();
          break;
        }
      }

      ImFlow::Pin* inPin = nullptr;
      for (auto& p : toNode->getIns()) {
        if (p->getUid() == sl.toPin) {
          inPin = p.get();
          break;
        }
      }

      if (!outPin || !inPin) continue;

      auto link = std::make_shared<ImFlow::Link>(outPin, inPin, &mINF);
      mINF.addLink(link);
    }

    // IMPORTANT: force pin creation
    for (auto& [uid, node] : uidToNode) {
      // INPUT PINS
      for (auto& inPin : node->getIns()) {
        auto weakLink = inPin->getLink();
        if (weakLink.lock().get()) weakLink.lock().get()->update();
      }

      // OUTPUT PINS
      for (auto& outPin : node->getOuts()) {
        auto weakLink = outPin->getLink();
        if (weakLink.lock().get()) weakLink.lock().get()->update();
      }
    }

    // ----------------------------
    // 4. FINALIZE
    // ----------------------------
    mINF.setSize(mSize);
  }

  void SaveToFile(const char* path) {
    SavedGraph graph = saveGraph(this->mINF);

    nlohmann::json j;
    j["nodes"] = nlohmann::json::array();
    j["links"] = nlohmann::json::array();

    for (auto& n : graph.nodes) {
      j["nodes"].push_back({{"uid", n.uid},
                            {"type", n.type},
                            {"x", n.pos.x},
                            {"y", n.pos.y},
                            {"data", n.data}});
    }

    for (auto& l : graph.links) {
      j["links"].push_back({{"fromNode", l.fromNode},
                            {"fromPin", l.fromPin},
                            {"toNode", l.toNode},
                            {"toPin", l.toPin}});
    }

    std::ofstream file(path);
    file << j.dump(2);
  }

  SavedGraph LoadFromFile(const char* path) {
    SavedGraph graph;

    std::ifstream file(path);
    if (!file.good()) return graph;

    nlohmann::json j;
    file >> j;

    for (auto& n : j["nodes"]) {
      graph.nodes.push_back({n["uid"].get<ImFlow::NodeUID>(),
                             n["type"].get<std::string>(),
                             ImVec2(n["x"], n["y"]), n["data"]});
    }

    for (auto& l : j["links"]) {
      SavedLink sl;
      sl.fromNode = l["fromNode"].get<ImFlow::NodeUID>();
      sl.fromPin = l["fromPin"].get<ImFlow::PinUID>();
      sl.toNode = l["toNode"].get<ImFlow::NodeUID>();
      sl.toPin = l["toPin"].get<ImFlow::PinUID>();

      graph.links.push_back(sl);
    }

    loadGraph(graph);

    return graph;
  }

 private:
  void handleContextMenus() {
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
        mINF.placeNodeAt<InputLayerNode>(openPopupMousePos);
      if (ImGui::MenuItem("Add Hidden Layer"))
        mINF.placeNodeAt<HiddenLayerNode>(openPopupMousePos);
      if (ImGui::MenuItem("Add Output Layer"))
        mINF.placeNodeAt<OutputLayerNode>(openPopupMousePos);
      if (ImGui::MenuItem("Add Activation Functions"))
        mINF.placeNodeAt<ActivationNode>(openPopupMousePos);
      if (ImGui::MenuItem("Add Neurons"))
        mINF.placeNodeAt<NeuronsNode>(openPopupMousePos);

      ImGui::Separator();

      if (ImGui::MenuItem("Clear All")) clear();

      ImGui::EndPopup();
    }
  }

  void linkDropped(ImFlow::Pin* droppedFrom) {
    if (!droppedFrom) return;
    if (!ImGui::IsPopupOpen("Background Context Menu"))
      ImGui::OpenPopup("Background Context Menu");
    drawBackgroundContextMenu();
  }

  void clear() {
    mINF = ImFlow::ImNodeFlow();
    mINF.setSize(mSize);
    mINF.droppedLinkPopUpContent(
        [this](ImFlow::Pin* fromPin) { linkDropped(fromPin); });
  }

  void updateModelFromGraph() {
    mModel = {};
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