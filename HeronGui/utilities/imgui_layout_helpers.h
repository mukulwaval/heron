#pragma once
#include <imgui.h>

namespace ImGui
{
    inline void BeginHorizontal(float spacing = -1.0f)
    {
        ImGui::BeginGroup(); // treat group as horizontal container
        if (spacing < 0.0f)
            spacing = ImGui::GetStyle().ItemSpacing.x;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(spacing, ImGui::GetStyle().ItemSpacing.y));
    }

    inline void EndHorizontal()
    {
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }

    inline void BeginVertical(float spacing = -1.0f)
    {
        ImGui::BeginGroup(); // treat group as vertical container
        if (spacing < 0.0f)
            spacing = ImGui::GetStyle().ItemSpacing.y;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, spacing));
    }

    inline void EndVertical()
    {
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }

    inline void Spring(float weight = 1.0f, float spacing = -1.0f)
    {
        float avail = ImGui::GetContentRegionAvail().x;

        if (avail > 0.0f)
            ImGui::Dummy(ImVec2(avail * weight, 0.0f));

        if (spacing < 0.0f)
        {
            // use default style spacing if spacing not provided
            spacing = ImGui::GetStyle().ItemSpacing.x;
        }

        ImGui::SameLine(0.0f, spacing);
    }
}
