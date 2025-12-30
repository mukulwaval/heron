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
#include <algorithm>

#include "application.h"
#include "source/platform.h"

#include <Heron/Network.h>
#include <Heron/Trainer.h>
#include <Heron/Loader.h>
#include <Heron/FileIO.h>

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

		void OnStop() override {}

		std::atomic<bool> training = false;
		std::atomic<bool> trainingDone = false;

		std::mutex logMutex;
		std::vector<std::string> logs;

		// ---- training progress ----
		std::mutex progressMutex;

		std::vector<float> accHistory;
		std::vector<int>   iterHistory;

		std::atomic<int>   currentIter = 0;
		std::atomic<float> currentAcc = 0.0f;

		// ---- evaluation state ----
		std::atomic<bool> evalRunning{ false };
		std::atomic<bool> evalDone{ false };

		std::atomic<float> evalAccuracy{ 0.0f };

		// ---- single sample eval ----
		std::atomic<bool> sampleRunning{ false };
		std::atomic<bool> sampleDone{ false };

		std::atomic<int> samplePred{ -1 };
		std::atomic<int> sampleLabel{ -1 };

		// ---- external image prediction ----
		std::atomic<bool> imgPredRunning{ false };
		std::atomic<bool> imgPredDone{ false };

		std::atomic<int> imgPredResult{ -1 };

		std::vector<float> imgPreview; // 28x28 grayscale

		void OnFrame(float deltaTime) override
		{
			// ---------- persistent UI state ----------
			static char modelPath[256] = "model.hrnmdl";
			static char evalModelPath[256] = "model.hrnmdl";
			static bool buildRequested = false;
			
			static int sampleIdx = 0;

			static char imgPath[256] = "images\\test_index.bmp";

			static int iters = 1000;
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
						ImVec2 size = ImGui::GetContentRegionAvail();
						Editor->set_size(size);
						Editor->draw();
					}

					ImGui::End();

					// ---------- Control ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Control");
					ImGui::PopFont();

					ImGui::Text("Model Save Path");
					ImGui::SetNextItemWidth(-1);
					ImGui::InputText("##model_path", modelPath, sizeof(modelPath));

					ImGui::Text("Model Load Path");
					ImGui::SetNextItemWidth(-1);
					ImGui::InputText("##eval_model_path", evalModelPath, sizeof(evalModelPath));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					ImGui::PushFont(MonoBold);
					ImGui::Text("Training Settings");
					ImGui::PopFont();

					ImGui::Text("Iterations");
					ImGui::SetNextItemWidth(-1);
					ImGui::InputInt("##iters", &iters);
					if (iters < 1) iters = 1;

					ImGui::Text("Learning Rate");
					ImGui::SetNextItemWidth(-1);
					ImGui::InputFloat("##lr", &lr, 0.001f, 0.01f, "%.6f");
					if (lr <= 0.0f) lr = 0.0001f;

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					if (!training && ImGui::Button("Build Model", ImVec2(-1, 0)))
						buildRequested = true;

					ImGui::Spacing();
					if (training) ImGui::TextColored(ImVec4(1, 1, 0, 1), "Training in progress...");
					if (trainingDone) ImGui::TextColored(ImVec4(0, 1, 0, 1), "Training finished.");

					ImGui::End();

					// ---------- Model Eval ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Model Evaluation");
					ImGui::PopFont();

					if (!evalRunning && ImGui::Button("Load & Evaluate", ImVec2(-1, 0)))
					{
						evalRunning = true;
						evalDone = false;
						evalAccuracy = 0.0f;

						{
							std::lock_guard lock(logMutex);
							logs.push_back("[EVAL] Starting evaluation...");
						}

						const std::string savedModelPath = evalModelPath;

						std::thread([this, savedModelPath]()
							{
								auto log = [&](const std::string& msg)
									{
										std::lock_guard lock(logMutex);
										logs.push_back(msg);
									};

								log("[EVAL] Loading dataset...");

								Heron::Loader loader("datasets");
								if (!loader.load("test") || loader.size() == 0)
								{
									log("[ERROR] Dataset load failed");
									evalRunning = false;
									evalDone = true;
									return;
								}

								std::vector<std::vector<float>> X;
								std::vector<uint8_t> Y;

								X.reserve(loader.size());
								Y.reserve(loader.size());

								for (size_t i = 0; i < loader.size(); ++i)
								{
									X.push_back(loader.get_image(i));
									Y.push_back(loader.get_label(i));
								}

								log("[EVAL] Loading model: " + savedModelPath);

								Heron::Network net;
								try {
									net.load_model(savedModelPath);
								}
								catch (...) {
									log("[ERROR] Failed to load model");
									evalRunning = false;
									evalDone = true;
									return;
								}

								std::vector<int> preds;
								preds.reserve(X.size());

								std::vector<std::vector<float>> Z, A;

								log("[EVAL] Running predictions...");

								for (auto& x : X)
								{
									net.predict(x, Z, A);
									preds.push_back(Heron::Trainer::get_prediction(A.back()));
								}

								float acc = Heron::Trainer::get_accuracy(preds, Y);
								evalAccuracy.store(acc, std::memory_order_relaxed);

								log("[EVAL] Accuracy = " + std::to_string(acc));

								evalRunning = false;
								evalDone = true;

							}).detach();
					}

					ImGui::Spacing();

					if (evalRunning) ImGui::TextColored(ImVec4(1, 1, 0, 1), "Status: %s", "Evaluating...");
					if (evalDone) ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: %s", "Done");
					else ImGui::Text("Status: %s", "Idle");

					ImGui::Text("Accuracy: %.4f", evalAccuracy.load());
					ImGui::End();

					// ---------- Image Prediction ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Image Prediction");
					ImGui::PopFont();

					ImGui::Text("Image Path (28x28 BMP)");
					ImGui::SetNextItemWidth(-1);
					ImGui::InputText("##img_path", imgPath, sizeof(imgPath));

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					if (!imgPredRunning && ImGui::Button("Predict Image", ImVec2(-1, 0)))
					{
						imgPredRunning = true;
						imgPredDone = false;
						imgPredResult = -1;

						{
							std::lock_guard lock(logMutex);
							logs.push_back("[IMG] Starting prediction...");
						}

						const std::string savedImgPath = imgPath;
						const std::string savedModelPath = evalModelPath;

						std::thread([this, savedImgPath, savedModelPath]()
							{
								auto log = [&](const std::string& m)
									{
										std::lock_guard lock(logMutex);
										logs.push_back(m);
									};

								try
								{
									imgPreview = Heron::FileIO::read_28x28_bmp(savedImgPath);
									log("[IMG] Image loaded");

									Heron::Network net;
									try {
										net.load_model(savedModelPath);
									}
									catch (...) {
										log("[ERROR] Failed to load model");
										imgPredRunning = false;
										imgPredDone = true;
										return;
									}

									std::vector<std::vector<float>> Z, A;
									net.predict(imgPreview, Z, A);

									int pred = Heron::Trainer::get_prediction(A.back());
									imgPredResult.store(pred, std::memory_order_relaxed);

									log("[IMG] Prediction = " + std::to_string(pred));
								}
								catch (const std::exception& e)
								{
									log(std::string("[ERROR] ") + e.what());
								}

								imgPredRunning = false;
								imgPredDone = true;

							}).detach();
					}

					ImGui::Spacing();

					if (imgPredRunning) ImGui::TextColored(ImVec4(1, 1, 0, 1), "Status: %s", "Running...");
					if (imgPredDone) ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: %s", "Done");
					else ImGui::Text("Status: %s", "Idle");

					if (imgPredDone && imgPredResult.load() != -1)
					{
						ImGui::TextColored(
							ImVec4(0, 1, 0, 1),
							"Prediction: %d",
							imgPredResult.load()
						);
					}
					ImGui::End();

					// ---------- Image Preview -----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Image Preview");
					ImGui::PopFont();

					if (!imgPreview.empty())
					{
						ImVec2 avail = ImGui::GetContentRegionAvail();
						float size = avail.x < avail.y ? avail.x : avail.y;
						float pixel = size / 28.0f;

						ImDrawList* draw = ImGui::GetWindowDrawList();

						ImVec2 origin = ImGui::GetCursorScreenPos();
						origin.x += (avail.x - size) * 0.5f;
						origin.y += (avail.y - size) * 0.5f;

						for (int y = 0; y < 28; ++y)
						{
							for (int x = 0; x < 28; ++x)
							{
								float v = imgPreview[y * 28 + x];
								int c = (int)(v * 255.0f);

								ImU32 col = IM_COL32(c, c, c, 255);

								ImVec2 a(
									origin.x + x * pixel,
									origin.y + y * pixel
								);
								ImVec2 b(
									a.x + pixel,
									a.y + pixel
								);

								draw->AddRectFilled(a, b, col);
							}
						}

						ImGui::Dummy(ImVec2(avail.x, avail.y));
					}
					else
					{
						ImGui::TextDisabled("No image loaded");
					}

					ImGui::End();

					// ---------- Data Sample ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Dataset Sample");
					ImGui::PopFont();

					ImGui::Text("Dataset Index");
					ImGui::SetNextItemWidth(-1);
					ImGui::InputInt("##sample_idx", &sampleIdx);
					if (sampleIdx < 0) sampleIdx = 0;
					if (sampleIdx > 9999) sampleIdx = 9999;

					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();

					if (!sampleRunning && ImGui::Button("Predict Sample", ImVec2(-1, 0)))
					{
						sampleRunning = true;
						sampleDone = false;
						samplePred = -1;
						sampleLabel = -1;

						{
							std::lock_guard lock(logMutex);
							logs.push_back("[SAMPLE] Starting prediction...");
						}

						const int savedIdx = sampleIdx;
						const std::string savedModelPath = evalModelPath;

						std::thread([this, savedIdx, savedModelPath]()
							{
								auto log = [&](const std::string& msg)
									{
										std::lock_guard lock(logMutex);
										logs.push_back(msg);
									};

								Heron::Loader loader("datasets");
								if (!loader.load("test") || loader.size() == 0)
								{
									log("[ERROR] Dataset load failed");
									sampleRunning = false;
									sampleDone = true;
									return;
								}

								if ((size_t)savedIdx >= loader.size())
								{
									log("[ERROR] Index out of range");
									sampleRunning = false;
									sampleDone = true;
									return;
								}

								std::vector<std::vector<float>> X;
								std::vector<uint8_t> Y;

								X.reserve(loader.size());
								Y.reserve(loader.size());

								for (size_t i = 0; i < loader.size(); ++i)
								{
									X.push_back(loader.get_image(i));
									Y.push_back(loader.get_label(i));
								}

								Heron::Network net;
								try {
									net.load_model(savedModelPath);
								}
								catch (...)
								{
									log("[ERROR] Failed to load model");
									sampleRunning = false;
									sampleDone = true;
									return;
								}

								std::vector<std::vector<float>> Z, A;
								net.predict(X[savedIdx], Z, A);

								int pred = Heron::Trainer::get_prediction(A.back());
								samplePred.store(pred, std::memory_order_relaxed);
								sampleLabel.store(Y[savedIdx], std::memory_order_relaxed);

								log("[SAMPLE] Prediction = " + std::to_string(pred));
								log("[SAMPLE] Label = " + std::to_string(Y[savedIdx]));

								Heron::FileIO::write_28x28_bmp(
									X[savedIdx],
									"images\\test_index.bmp"
								);

								log("[SAMPLE] Image saved to images\\test_index.bmp");

								sampleRunning = false;
								sampleDone = true;

							}).detach();
					}

					ImGui::Spacing();

					if (sampleRunning) ImGui::TextColored(ImVec4(1, 1, 0, 1), "Status: %s", "Running...");
					if (sampleDone) ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: %s", "Done");
					else ImGui::Text("Status: %s", "Idle");

					if (sampleDone && samplePred.load() != -1)
					{
						ImGui::TextColored(ImVec4(0, 1, 0, 1),
							"Prediction: %d", samplePred.load());
						ImGui::Text(
							"Label: %d", sampleLabel.load());
					}
					ImGui::End();

					// ---------- Sample Image Display ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Sample Image");
					ImGui::PopFont();

					static std::vector<float> previewImg;

					if (sampleDone)
					{
						try {
							previewImg = Heron::FileIO::read_28x28_bmp("images\\test_index.bmp");
						}
						catch (...) {
							ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load image");
						}
					}

					if (!previewImg.empty())
					{
						ImVec2 avail = ImGui::GetContentRegionAvail();

						// keep it square
						float size = (avail.x < avail.y) ? avail.x : avail.y;
						float pixel = size / 28.0f;

						ImDrawList* draw = ImGui::GetWindowDrawList();

						// center it
						ImVec2 origin = ImGui::GetCursorScreenPos();
						origin.x += (avail.x - size) * 0.5f;
						origin.y += (avail.y - size) * 0.5f;

						for (int y = 0; y < 28; ++y)
						{
							for (int x = 0; x < 28; ++x)
							{
								float v = previewImg[y * 28 + x];
								int c = (int)(v * 255.0f);

								ImU32 col = IM_COL32(c, c, c, 255);

								ImVec2 a(
									origin.x + x * pixel,
									origin.y + y * pixel
								);
								ImVec2 b(
									a.x + pixel,
									a.y + pixel
								);

								draw->AddRectFilled(a, b, col);
							}
						}

						// reserve layout space so ImGui behaves
						ImGui::Dummy(ImVec2(avail.x, avail.y));
					}
					else
					{
						ImGui::TextDisabled("No image loaded");
					}

					ImGui::End();


					// ---------- Model Debug ----------
					const Nodes::Model* model = Editor ? Editor->getModel() : nullptr;

					if (model)
					{
						ImGui::PushFont(BoldItalic);
						ImGui::Begin("Model Debug");
						ImGui::PopFont();

						ImGui::Text("Layers:");
						for (size_t i = 0; i < model->layers.size(); ++i)
							ImGui::BulletText("Layer %zu: %zu neurons", i, model->layers[i]);

						ImGui::Separator();
						ImGui::Text("Activations:");
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

					// ---------- Model Training ----------
					if (buildRequested && model && !training)
					{
						buildRequested = false;
						training = true;
						trainingDone = false;

						{
							std::lock_guard lock(progressMutex);
							accHistory.clear();
							iterHistory.clear();
						}
						currentIter = 0;
						currentAcc = 0.0f;

						// copy everything the thread needs
						const std::string savedPath = modelPath;
						const std::vector<size_t> savedLayers = model->layers;

						std::vector<
							std::vector<float>(*)(const std::vector<float>&)
						> savedActivations;

						for (size_t i = 1; i < model->activations.size(); ++i)
							savedActivations.push_back(model->activations[i].fn);

						const int savedIters = iters;
						const float savedLr = lr;

						std::thread([this,
							savedPath,
							savedLayers,
							savedActivations,
							savedIters,
							savedLr
						]()
							{
								auto log = [&](const std::string& msg)
									{
										std::lock_guard<std::mutex> lock(logMutex);
										logs.push_back(msg);
									};

								log("[BUILD] Background training started");

								Heron::Loader loader("datasets");
								if (!loader.load("train") || loader.size() == 0)
								{
									log("[ERROR] Dataset load failed");
									training = false;
									trainingDone = true;
									return;
								}

								std::vector<std::vector<float>> X;
								std::vector<uint8_t> Y;

								X.reserve(loader.size());
								Y.reserve(loader.size());

								for (size_t i = 0; i < loader.size(); ++i)
								{
									X.push_back(loader.get_image(i));
									Y.push_back(loader.get_label(i));
								}

								if (savedLayers.size() < 2 ||
									savedActivations.size() != savedLayers.size() - 1)
								{
									log("[ERROR] Invalid model definition");
									training = false;
									trainingDone = true;
									return;
								}

								log("[BUILD] Training network...");
								Heron::Network net(savedLayers, savedActivations);
								Heron::Trainer::gradient_descent(
									net,
									X,
									Y,
									savedLr,
									savedIters,
									[this]
									(int iter, float acc)
									{
										{
											std::lock_guard lock(progressMutex);
											iterHistory.push_back(iter);
											accHistory.push_back(acc);
										}

										currentIter = iter;
										currentAcc = acc;

										{
											std::lock_guard lock(logMutex);
											logs.push_back(
												"[ITER " + std::to_string(iter) +
												"] acc = " + std::to_string(acc)
											);
										}
									}
								);

								net.save_model(savedPath);
								log("[BUILD] Model saved: " + savedPath);

								training = false;
								trainingDone = true;

							}).detach();
					}


					// ---------- Plot ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Training Progress");
					ImGui::PopFont();

					if (training) ImGui::TextColored(ImVec4(1, 1, 0, 1), "Status: %s", "Training...");
					if (trainingDone) ImGui::TextColored(ImVec4(0, 1, 0, 1), "Status: %s", "Done");
					else ImGui::Text("Status: %s", "Idle");

					ImGui::Text("Iteration: %d", currentIter.load());
					ImGui::Text("Accuracy : %.4f", currentAcc.load());

					ImGui::Separator();

					{
						std::lock_guard lock(progressMutex);

						if (!accHistory.empty())
						{
							ImGui::PlotLines(
								"Accuracy",
								accHistory.data(),
								(int)accHistory.size(),
								0,
								nullptr,
								0.0f,
								1.0f,
								ImVec2(-1, 200)
							);
						}
						else
						{
							ImGui::TextDisabled("Waiting for training data...");
						}
					}

					ImGui::End();

					// ---------- Log ----------
					ImGui::PushFont(BoldItalic);
					ImGui::Begin("Log");
					ImGui::PopFont();

					ImGui::BeginChild("log_scroll", ImVec2(0, 0), false);

					{
						std::lock_guard lock(logMutex);
						for (const auto& line : logs)
							ImGui::TextUnformatted(line.c_str());
					}

					if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);

					ImGui::EndChild();
					ImGui::End();
				});
		}
	};
}

int Main(int argc, char** argv) {
	HeronGui::MainEditor editor("editor", argc, argv);

	if (editor.Create()) return editor.Run();

	return 0;
}