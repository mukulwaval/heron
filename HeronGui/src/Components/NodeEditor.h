#pragma once
#include <imgui_node_editor.h>

namespace HeronGui::Component::NodeEditor
{
	namespace ed = ax::NodeEditor;

	void Init();
	void Shutdown();
	ed::EditorContext* Get();

	static ed::NodeId NodeIdFromIndex(int i) { return ed::NodeId(i + 1); }
	static ed::PinId  InputPinId(int i) { return ed::PinId(1000 + i); }
	static ed::PinId  OutputPinId(int i) { return ed::PinId(2000 + i); }
}
