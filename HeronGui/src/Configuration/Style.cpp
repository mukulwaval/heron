#include "Style.h"
#include "imgui.h"

namespace HeronGui::Configuration::Style
{
	void SetupStyle()
	{
		// Deep Dark style by janekb04 from ImThemes
		ImGuiStyle& style = ImGui::GetStyle();

		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.WindowRounding = 7.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = ImVec2(32.0f, 32.0f);
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.ChildRounding = 4.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 4.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = ImVec2(5.0f, 5.0f);
		style.FrameRounding = 3.0f;
		style.FrameBorderSize = 1.0f;
		style.ItemSpacing = ImVec2(6.0f, 6.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
		style.CellPadding = ImVec2(6.0f, 6.0f);
		style.IndentSpacing = 25.0f;
		style.ColumnsMinSpacing = 6.0f;
		style.ScrollbarSize = 15.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 10.0f;
		style.GrabRounding = 3.0f;
		style.TabRounding = 4.0f;
		style.TabBorderSize = 1.0f;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

		style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.49803922f, 0.49803922f, 0.49803922f, 1.0f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1882353f, 0.1882353f, 0.1882353f, 0.92f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.1882353f, 0.1882353f, 0.1882353f, 0.29f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.24f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.047058824f, 0.047058824f, 0.047058824f, 0.54f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1882353f, 0.1882353f, 0.1882353f, 0.54f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2f, 0.21960784f, 0.22745098f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05882353f, 0.05882353f, 0.05882353f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.047058824f, 0.047058824f, 0.047058824f, 0.54f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3372549f, 0.3372549f, 0.3372549f, 0.54f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.54f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5568628f, 0.5568628f, 0.5568628f, 0.54f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.32941177f, 0.6666667f, 0.85882354f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3372549f, 0.3372549f, 0.3372549f, 0.54f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5568628f, 0.5568628f, 0.5568628f, 0.54f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.047058824f, 0.047058824f, 0.047058824f, 0.54f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1882353f, 0.1882353f, 0.1882353f, 0.54f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2f, 0.21960784f, 0.22745098f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.36f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2f, 0.21960784f, 0.22745098f, 0.33f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.2784314f, 0.2784314f, 0.2784314f, 0.29f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.4392157f, 0.4392157f, 0.4392157f, 0.29f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.4f, 0.4392157f, 0.46666667f, 1.0f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2784314f, 0.2784314f, 0.2784314f, 0.29f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4392157f, 0.4392157f, 0.4392157f, 0.29f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4f, 0.4392157f, 0.46666667f, 1.0f);
		style.Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.2f, 0.2f, 0.36f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
		style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
		style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2784314f, 0.2784314f, 0.2784314f, 0.29f);
		style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2f, 0.21960784f, 0.22745098f, 1.0f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.32941177f, 0.6666667f, 0.85882354f, 1.0f);
		style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 0.7f);
		style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.2f);
		style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.35f);
	}
}
