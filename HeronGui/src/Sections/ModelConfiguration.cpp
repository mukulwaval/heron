#include "ModelConfiguration.h"
#include "AppState.h"
#include "imgui.h"
#include <fmt/format.h>
#include "Configuration/Style.h"
#include "Components/NodeEditor.h"
#include "Configuration/Font.h"
#include <imgui_node_editor.h>

#include <vector>

namespace HeronGui::Sections::ModelConfiguration
{
	namespace ed = ax::NodeEditor;

	void RenderModelConfigSection(AppState& state)
	{
		ImGui::Begin("Model Configuration");

		ed::SetCurrentEditor(HeronGui::Component::NodeEditor::Get());
		ed::Begin("ModelGraph");

		auto& layers = state.model.layers;

		if (layers.empty())
		{
			layers.push_back({ 784, ActType::None });
			layers.push_back({ 10, ActType::Softmax });

			state.model.dirty = true;
		}

		for (int i = 0; i < (int)layers.size(); ++i)
		{
			ed::BeginNode(HeronGui::Component::NodeEditor::NodeIdFromIndex(i));
			ImGui::PushID(i);

			// ---- TITLE ----
			ImGui::Text("Layer %d", i + 1);

			// ---- INPUT PIN (LEFT) ----
			//if (i > 0)
			//{
				ed::BeginPin(
					HeronGui::Component::NodeEditor::InputPinId(i),
					ed::PinKind::Input
				);
				ImGui::Text("In");
				ed::EndPin();
			//}

			auto& io = ImGui::GetIO();
			ed::EnableShortcuts(!io.WantTextInput);

			// ---- CENTER CONTENT ----
			ImGui::PushItemWidth(200);
			//ImGui::SameLine();
			ImGui::BeginGroup();

			if (ImGui::InputInt("##neurons", &layers[i].size))
				state.model.dirty = true;

			int act = (int)layers[i].activation;
			if (ImGui::Combo("##activation", &act, ActNames, IM_ARRAYSIZE(ActNames)))
			{
				layers[i].activation = (ActType)act;
				state.model.dirty = true;
			}

			ImGui::EndGroup();
			ImGui::PopItemWidth();

			// ---- OUTPUT PIN (RIGHT) ----
			//if (i < (int)layers.size() - 1)
			//{
			//ImGui::SameLine();
			ImGui::Dummy(ImVec2(250, 0)); // spacing trick (Blueprint style)
			//ImGui::SameLine();

			ed::BeginPin(
				HeronGui::Component::NodeEditor::OutputPinId(i),
				ed::PinKind::Output
			);
			ImGui::Text("Out");
			ed::EndPin();
			//}

			ImGui::PopID();
			ed::EndNode();
		}

		// Draw links
		for (int i = 0; i < (int)layers.size() - 1; ++i)
		{
			ed::Link(
				ed::LinkId(3000 + i),
				HeronGui::Component::NodeEditor::OutputPinId(i),
				HeronGui::Component::NodeEditor::InputPinId(i + 1)
			);
		}

		ed::End();
		ed::SetCurrentEditor(nullptr);

		ImGui::End();
	}
}
