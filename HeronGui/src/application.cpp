#include "application.h"
#include "imgui.h"
#include "fonts.h"
#include <Heron.h>

#include <functional>

namespace HeronGui
{
	void RenderUI() {
		InitDockspace([]() {
			ImGui::Begin("hi");
			ImGui::Button("Hello");
			static float v = 50.0f;
			ImGui::DragFloat("float", &v);
			ImGui::End();
			});

		ImGui::ShowDemoWindow();

	}

	void LoadApplicationFonts()
	{
		ImGuiIO& io = ImGui::GetIO();

		// UI font (Roboto Regular)
		UI_Normal = io.Fonts->AddFontFromFileTTF(
			"assets/fonts/Roboto-VariableFont_wdth,wght.ttf",
			18.0f
		);

		// UI font (Roboto Italic)
		UI_Italic = io.Fonts->AddFontFromFileTTF(
			"assets/fonts/Roboto-Italic-VariableFont_wdth,wght.ttf",
			18.0f
		);

		// Code / logs font (JetBrains Mono Regular)
		Mono_Normal = io.Fonts->AddFontFromFileTTF(
			"assets/fonts/JetBrainsMono-VariableFont_wght.ttf",
			16.0f
		);

		// Code / logs font (JetBrains Mono Italic)
		Mono_Italic = io.Fonts->AddFontFromFileTTF(
			"assets/fonts/JetBrainsMono-Italic-VariableFont_wght.ttf",
			16.0f
		);

		// Default font = Roboto
		io.FontDefault = Mono_Normal;

		// Safety checks
		IM_ASSERT(UI_Normal && "Failed to load Roboto Regular");
		IM_ASSERT(Mono_Normal && "Failed to load JetBrains Mono");
	}

	void SetupImGuiStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec4* colors = style.Colors;

		ImVec4 CYAN = ImVec4(0.20f, 0.75f, 0.75f, 1.00f);
		ImVec4 CYAN_SOFT = ImVec4(0.20f, 0.75f, 0.75f, 0.70f);
		ImVec4 CYAN_FAINT = ImVec4(0.20f, 0.75f, 0.75f, 0.40f);
		ImVec4 CYAN_DIM = ImVec4(0.15f, 0.45f, 0.45f, 0.86f);

		colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.35f, 0.35f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = CYAN_FAINT;
		colors[ImGuiCol_FrameBgActive] = CYAN_SOFT;

		colors[ImGuiCol_TitleBgActive] = ImVec4(0.04f, 0.12f, 0.12f, 1.00f);

		colors[ImGuiCol_CheckMark] = CYAN;
		colors[ImGuiCol_SliderGrab] = CYAN_DIM;
		colors[ImGuiCol_SliderGrabActive] = CYAN;

		colors[ImGuiCol_Button] = CYAN_FAINT;
		colors[ImGuiCol_ButtonHovered] = CYAN;
		colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.65f, 0.65f, 1.00f);

		colors[ImGuiCol_Header] = ImVec4(0.20f, 0.75f, 0.75f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = CYAN_SOFT;
		colors[ImGuiCol_HeaderActive] = CYAN;

		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.15f, 0.65f, 0.65f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = CYAN;

		colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.75f, 0.75f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = CYAN_SOFT;
		colors[ImGuiCol_ResizeGripActive] = CYAN;

		colors[ImGuiCol_TabHovered] = CYAN_SOFT;
		colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.30f, 0.30f, 0.86f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.15f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_TabSelectedOverline] = CYAN;

		colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.16f, 0.16f, 1.00f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.08f, 0.26f, 0.26f, 1.00f);

		colors[ImGuiCol_DockingPreview] = CYAN_SOFT;

		colors[ImGuiCol_TextLink] = CYAN;
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.75f, 0.75f, 0.35f);

		colors[ImGuiCol_NavCursor] = CYAN;

		style.FrameRounding = 6.0f;
		style.TabRounding = 6.0f;
		style.GrabRounding = 6.0f;
		style.WindowRounding = 8.0f;
	}

	void InitDockspace(const std::function<void()>& render_content)
	{
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// TODO: remove this later if needed
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;

		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace itself
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (render_content)
			render_content();

		ImGui::End();
	}

}
