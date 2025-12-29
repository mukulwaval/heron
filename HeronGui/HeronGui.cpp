#define STB_IMAGE_IMPLEMENTATION

#ifdef HRN_BUILD_DLL
#error HRN_BUILD_DLL SHOULD NOT BE DEFINED IN HERONGUI
#endif

#include <ImNodeFlow.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <memory>
#include <atomic>
#include <mutex>

#include "application.h"
#include "source/platform.h"

#include <Heron/Network.h>
#include <Heron/Trainer.h>
#include <Heron/Loader.h>

#include "utilities/nodes.h"
#include "utilities/components.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef __linux__
#include <GLFW/glfw3.h>

#include "stb_image.h"
#endif

#include "resource.h"

namespace HeronGui
{
	struct MainEditor : public Application {
		std::unique_ptr<Nodes::NodeEditor> Editor;

		MainEditor(const char* name, int argc, char** argv)
			: Application(name, argc, argv)  // forward to base
		{
		}

		void LoadFonts() {
			ImGuiIO& io = ImGui::GetIO();
			const float baseSize = 17.0f;

			ImFontConfig cfg;
			cfg.GlyphOffset = ImVec2(0.0f, 0.0f);

			// ---- Google Sans ----
			io.FontDefault = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans/static/GoogleSans-Regular.ttf", baseSize,
				&cfg);
			Italic = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans/static/GoogleSans-Italic.ttf", baseSize,
				&cfg);
			Medium = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans/static/GoogleSans-Medium.ttf", baseSize,
				&cfg);
			MediumItalic = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans/static/GoogleSans-MediumItalic.ttf", baseSize,
				&cfg);
			SemiBold = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans/static/GoogleSans-SemiBold.ttf", baseSize,
				&cfg);
			SemiBoldItalic = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans/static/GoogleSans-SemiBoldItalic.ttf",
				baseSize, &cfg);
			Bold = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans/static/GoogleSans-Bold.ttf", baseSize, &cfg);
			BoldItalic = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans/static/GoogleSans-BoldItalic.ttf", baseSize,
				&cfg);

			// ---- Google Sans Code (Mono) ----
			Mono = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans_Code/static/GoogleSansCode-Regular.ttf",
				baseSize, &cfg);
			MonoItalic = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans_Code/static/GoogleSansCode-Italic.ttf",
				baseSize, &cfg);
			MonoMedium = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans_Code/static/GoogleSansCode-Medium.ttf",
				baseSize, &cfg);
			MonoMediumItalic = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans_Code/static/GoogleSansCode-MediumItalic.ttf",
				baseSize, &cfg);
			MonoSemiBold = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans_Code/static/GoogleSansCode-SemiBold.ttf",
				baseSize, &cfg);
			MonoSemiBoldItalic = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans_Code/static/"
				"GoogleSansCode-SemiBoldItalic.ttf",
				baseSize, &cfg);
			MonoBold = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans_Code/static/GoogleSansCode-Bold.ttf",
				baseSize, &cfg);
			MonoBoldItalic = io.Fonts->AddFontFromFileTTF(
				"assets/fonts/Google_Sans_Code/static/GoogleSansCode-BoldItalic.ttf",
				baseSize, &cfg);
		}

		void SetupStyle() {
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
			style.Colors[ImGuiCol_TextDisabled] =
				ImVec4(0.49803922f, 0.49803922f, 0.49803922f, 1.0f);
			style.Colors[ImGuiCol_WindowBg] =
				ImVec4(0.09803922f, 0.09803922f, 0.09803922f, 1.0f);
			style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
			style.Colors[ImGuiCol_PopupBg] =
				ImVec4(0.1882353f, 0.1882353f, 0.1882353f, 0.92f);
			style.Colors[ImGuiCol_Border] =
				ImVec4(0.1882353f, 0.1882353f, 0.1882353f, 0.29f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.24f);
			style.Colors[ImGuiCol_FrameBg] =
				ImVec4(0.047058824f, 0.047058824f, 0.047058824f, 0.54f);
			style.Colors[ImGuiCol_FrameBgHovered] =
				ImVec4(0.1882353f, 0.1882353f, 0.1882353f, 0.54f);
			style.Colors[ImGuiCol_FrameBgActive] =
				ImVec4(0.2f, 0.21960784f, 0.22745098f, 1.0f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			style.Colors[ImGuiCol_TitleBgActive] =
				ImVec4(0.05882353f, 0.05882353f, 0.05882353f, 1.0f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			style.Colors[ImGuiCol_MenuBarBg] =
				ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
			style.Colors[ImGuiCol_ScrollbarBg] =
				ImVec4(0.047058824f, 0.047058824f, 0.047058824f, 0.54f);
			style.Colors[ImGuiCol_ScrollbarGrab] =
				ImVec4(0.3372549f, 0.3372549f, 0.3372549f, 0.54f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] =
				ImVec4(0.4f, 0.4f, 0.4f, 0.54f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] =
				ImVec4(0.5568628f, 0.5568628f, 0.5568628f, 0.54f);
			style.Colors[ImGuiCol_CheckMark] =
				ImVec4(0.32941177f, 0.6666667f, 0.85882354f, 1.0f);
			style.Colors[ImGuiCol_SliderGrab] =
				ImVec4(0.3372549f, 0.3372549f, 0.3372549f, 0.54f);
			style.Colors[ImGuiCol_SliderGrabActive] =
				ImVec4(0.5568628f, 0.5568628f, 0.5568628f, 0.54f);
			style.Colors[ImGuiCol_Button] =
				ImVec4(0.047058824f, 0.047058824f, 0.047058824f, 0.54f);
			style.Colors[ImGuiCol_ButtonHovered] =
				ImVec4(0.1882353f, 0.1882353f, 0.1882353f, 0.54f);
			style.Colors[ImGuiCol_ButtonActive] =
				ImVec4(0.2f, 0.21960784f, 0.22745098f, 1.0f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.36f);
			style.Colors[ImGuiCol_HeaderActive] =
				ImVec4(0.2f, 0.21960784f, 0.22745098f, 0.33f);
			style.Colors[ImGuiCol_Separator] =
				ImVec4(0.2784314f, 0.2784314f, 0.2784314f, 0.29f);
			style.Colors[ImGuiCol_SeparatorHovered] =
				ImVec4(0.4392157f, 0.4392157f, 0.4392157f, 0.29f);
			style.Colors[ImGuiCol_SeparatorActive] =
				ImVec4(0.4f, 0.4392157f, 0.46666667f, 1.0f);
			style.Colors[ImGuiCol_ResizeGrip] =
				ImVec4(0.2784314f, 0.2784314f, 0.2784314f, 0.29f);
			style.Colors[ImGuiCol_ResizeGripHovered] =
				ImVec4(0.4392157f, 0.4392157f, 0.4392157f, 0.29f);
			style.Colors[ImGuiCol_ResizeGripActive] =
				ImVec4(0.4f, 0.4392157f, 0.46666667f, 1.0f);
			style.Colors[ImGuiCol_Tab] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
			style.Colors[ImGuiCol_TabHovered] =
				ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
			style.Colors[ImGuiCol_TabActive] = ImVec4(0.2f, 0.2f, 0.2f, 0.36f);
			style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
			style.Colors[ImGuiCol_TabUnfocusedActive] =
				ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			style.Colors[ImGuiCol_PlotHistogramHovered] =
				ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
			style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0f, 0.0f, 0.0f, 0.52f);
			style.Colors[ImGuiCol_TableBorderLight] =
				ImVec4(0.2784314f, 0.2784314f, 0.2784314f, 0.29f);
			style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
			style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
			style.Colors[ImGuiCol_TextSelectedBg] =
				ImVec4(0.2f, 0.21960784f, 0.22745098f, 1.0f);
			style.Colors[ImGuiCol_DragDropTarget] =
				ImVec4(0.32941177f, 0.6666667f, 0.85882354f, 1.0f);
			style.Colors[ImGuiCol_NavHighlight] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			style.Colors[ImGuiCol_NavWindowingHighlight] =
				ImVec4(1.0f, 0.0f, 0.0f, 0.7f);
			style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.2f);
			style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.35f);
		}

		void OnStart() override {
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

			SetTitle("Heron");

#ifdef _WIN32
			auto hwnd = (HWND)m_Platform->GetNativeWindowHandle();
			if (hwnd) {
				HICON hIcon =
					LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON1));
				SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
				SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			}
#endif

#ifdef __linux__
			if (auto window = (GLFWwindow*)m_Platform->GetMainWindowHandle()) {
				int w, h, channels;
				unsigned char* pixels =
					stbi_load("assets/app.png", &w, &h, &channels, 4);  // RGBA
				if (pixels) {
					GLFWimage icon;
					icon.width = w;
					icon.height = h;
					icon.pixels = pixels;
					glfwSetWindowIcon(window, 1, &icon);
					stbi_image_free(pixels);
				}
			}
#endif

			ImGui::StyleColorsDark();  // base style
			LoadFonts();
			SetupStyle();

			ImGui::GetIO().ConfigDpiScaleFonts = true;
			ImGui::GetIO().ConfigDpiScaleViewports = true;
		}

		void OnStop() override {
		}

		void OnFrame(float deltaTime) override
		{
			// --- persistent UI state ---
			static char modelPath[256] = "model.heron";
			static bool buildRequested = false;
			static std::vector<std::string> consoleLogs;

			static int iters = 50;
			static float lr = 0.01f;

			Components::Dockspace([&]()
				{
					// ---------- Node Editor ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Node Editor");
					ImGui::PopFont();

					if (!Editor)
						Editor = std::make_unique<Nodes::NodeEditor>(500.0f);

					if (Editor)
					{
						ImVec2 editor_size = ImGui::GetContentRegionAvail();
						Editor->set_size(editor_size);
						Editor->draw();
					}

					ImGui::End();

					// ---------- Control Window ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Control");
					ImGui::PopFont();

					ImGui::PushFont(MonoBold);
					ImGui::Text("Model Path");
					ImGui::PopFont();
					ImGui::SetNextItemWidth(-1);
					ImGui::InputText("##model_path", modelPath, sizeof(modelPath));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::PushFont(MonoBold);
					ImGui::Text("Training Settings");
					ImGui::PopFont();

					// Iterations
					ImGui::PushFont(MonoSemiBold);
					ImGui::Text("Iterations");
					ImGui::PopFont();
					ImGui::SetNextItemWidth(-1);
					ImGui::InputInt("##iters", &iters);
					if (iters < 1) iters = 1; // clamp, no goofy values

					ImGui::Spacing();

					// Learning rate
					ImGui::PushFont(MonoSemiBold);
					ImGui::Text("Learning Rate");
					ImGui::PopFont();
					ImGui::SetNextItemWidth(-1);
					ImGui::InputFloat("##lr", &lr, 0.001f, 0.01f, "%.6f");
					if (lr <= 0.0f) lr = 0.0001f;

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					if (ImGui::Button("Build Model", ImVec2(-1, 0)))
						buildRequested = true;

					ImGui::End();

					// ---------- Model Debug ----------
					const Nodes::Model* model = nullptr;
					if (Editor)
						model = Editor->getModel();

					if (model)
					{
						ImGui::PushFont(BoldItalic);
						ImGui::Begin("Model Debug");
						ImGui::PopFont();

						ImGui::PushFont(MonoBold);
						ImGui::Text("Layers:");
						ImGui::PopFont();
						for (size_t i = 0; i < model->layers.size(); ++i)
						{
							ImGui::BulletText(
								"Layer %zu: %zu neurons",
								i,
								model->layers[i]
							);
						}

						ImGui::Separator();

						ImGui::PushFont(MonoBold);
						ImGui::Text("Activations:");
						ImGui::PopFont();
						for (size_t i = 0; i < model->activations.size(); ++i)
						{
							ImGui::BulletText(
								"Layer %zu: %s",
								i,
								model->activations[i].name
								? model->activations[i].name
								: "None"
							);
						}

						ImGui::End();
					}

					// ---------- Build + Console logging ----------
					if (buildRequested && model)
					{
						buildRequested = false;

						// Console logging
						char buf[256];

						snprintf(buf, sizeof(buf),
							"[BUILD] Path: %s",
							modelPath);
						consoleLogs.emplace_back(buf);
						printf("%s\n", buf);

						snprintf(buf, sizeof(buf),
							"[BUILD] Layers: %zu",
							model->layers.size());
						consoleLogs.emplace_back(buf);
						printf("%s\n", buf);


						// Load dataset
						Heron::Loader loader("datasets");

						std::vector<std::vector<float>> X;
						std::vector<uint8_t> Y;

						if (!loader.load("train"))
						{
							consoleLogs.emplace_back("[ERROR] Failed to load training dataset");
							printf("[ERROR] Failed to load training dataset\n");
							return;
						}

						X.reserve(loader.size());
						Y.reserve(loader.size());

						for (size_t i = 0; i < loader.size(); ++i)
						{
							X.push_back(loader.get_image(i));
							Y.push_back(loader.get_label(i));
						}

						// Build network from node graph
						std::vector<size_t> layer_sizes;
						std::vector<std::vector<float>(*)(const std::vector<float>&)> activations;

						// layers come straight from graph
						for (size_t l : model->layers)
							layer_sizes.push_back(l);

						// activations: skip input layer
						for (size_t i = 1; i < model->activations.size(); i++) // skip index 0.
						{
							activations.push_back(model->activations[i].fn);
						}

						// sanity check
						if (layer_sizes.size() < 2)
						{
							consoleLogs.emplace_back("[ERROR] Model must have at least 2 layers");
							printf("[ERROR] Model must have at least 2 layers\n");
							return;
						}

						if (activations.size() != layer_sizes.size() - 1)
						{
							consoleLogs.emplace_back("[ERROR] Activation count mismatch");
							printf("[ERROR] Activation count mismatch\n");
							return;
						}

						// Train
						Heron::Network net(layer_sizes, activations);
						Heron::Trainer::gradient_descent(net, X, Y, lr, iters);

						// Save
						net.save_model(modelPath);

						consoleLogs.emplace_back("[BUILD] Model trained & saved");
						printf("[BUILD] Model trained & saved\n");
					}

					// ---------- Console Window ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Console");
					ImGui::PopFont();

					for (const auto& line : consoleLogs)
						ImGui::TextUnformatted(line.c_str());

					if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);

					ImGui::End();

				});
			ImGui::ShowDemoWindow();
		}
	};
}

int Main(int argc, char** argv) {
	HeronGui::MainEditor editor("editor", argc, argv);

	if (editor.Create()) return editor.Run();

	return 0;
}