#include "Components/NodeEditor.h"
#include <imgui.h>
#include <map>

namespace HeronGui::Component::NodeEditor
{
    ed::EditorContext* g_Context = nullptr;

    static std::map<int, ImVec2> nodePos;

    void Init()
    {
        if (!g_Context)
        {
            ed::Config config;
            config.SettingsFile = nullptr;
            g_Context = ed::CreateEditor(&config);
        }
    }

    void Shutdown()
    {
        if (g_Context)
        {
            ed::DestroyEditor(g_Context);
            g_Context = nullptr;
        }
    }

    ed::EditorContext* Get() { return g_Context; }

    ed::NodeId MakeNodeId(int index) { return ed::NodeId(index + 1); }
    ed::PinId  MakeInputPinId(int index) { return ed::PinId((index + 1) * 10 + 0); }
    ed::PinId  MakeOutputPinId(int index) { return ed::PinId((index + 1) * 10 + 1); }

    void SaveNodePosition(ed::NodeId id, const ImVec2& pos)
    {
        nodePos[(int)id.Get()] = pos;
    }

    ImVec2 GetNodePosition(ed::NodeId id)
    {
        auto it = nodePos.find((int)id.Get());
        if (it != nodePos.end())
            return it->second;
        return ImVec2(0, 0);
    }
}
