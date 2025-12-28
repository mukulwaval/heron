#define STB_IMAGE_IMPLEMENTATION

#include <ImNodeFlow.h>
#include <imgui.h>
#include <imgui_internal.h>

#include "application.h"
#include "source/platform.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef __linux__
#include <GLFW/glfw3.h>

#include "stb_image.h"
#endif

#include "resource.h"

class SimpleSum : public ImFlow::BaseNode {
 public:
  SimpleSum() {
    setTitle("Simple sum");
    setStyle(ImFlow::NodeStyle::green());
    ImFlow::BaseNode::addIN<int>("In", 0, ImFlow::ConnectionFilter::SameType());
    ImFlow::BaseNode::addOUT<int>("Out", nullptr)->behaviour([this]() {
      return getInVal<int>("In") + m_valB;
    });
  }

  void draw() override {
    ImGui::SetNextItemWidth(100.f);
    ImGui::InputInt("##ValB", &m_valB);
  }

 private:
  int m_valB = 0;
};

class CollapsingNode : public ImFlow::BaseNode {
 public:
  CollapsingNode() {
    setTitle("Collapsing node");
    setStyle(ImFlow::NodeStyle::red());
    ImFlow::BaseNode::addIN<int>("A", 0, ImFlow::ConnectionFilter::SameType());
    ImFlow::BaseNode::addIN<int>("B", 0, ImFlow::ConnectionFilter::SameType());
    ImFlow::BaseNode::addOUT<int>("Out", nullptr)->behaviour([this]() {
      return getInVal<int>("A") + getInVal<int>("B");
    });
  }

  void draw() override {
    if (ImFlow::BaseNode::isSelected()) {
      ImGui::SetNextItemWidth(100.f);
      ImGui::Text("You can only see me when the node is selected!");
    }
  }
};

class ResultNode : public ImFlow::BaseNode {
 public:
  ResultNode() {
    setTitle("Result node");
    setStyle(ImFlow::NodeStyle::brown());
    ImFlow::BaseNode::addIN<int>("A", 0, ImFlow::ConnectionFilter::SameType());
    ImFlow::BaseNode::addIN<int>("B", 0, ImFlow::ConnectionFilter::SameType());
  }

  void draw() override {
    ImGui::Text("Result: %d", getInVal<int>("A") + getInVal<int>("B"));
  }
};

struct NodeEditor : ImFlow::BaseNode {
  ImFlow::ImNodeFlow mINF;

  NodeEditor(float d, std::size_t r) : BaseNode() {
    mINF.setSize({d, d});
    if (r > 0) {
      auto n1 = mINF.addNode<SimpleSum>({40, 40});
      auto n2 = mINF.addNode<SimpleSum>({40, 150});
      auto result = mINF.addNode<ResultNode>({250, 80});

      // Add links between nodes
      n1->outPin("Out")->createLink(result->inPin("A"));
      n2->outPin("Out")->createLink(result->inPin("B"));

      // Add a collapsing node
      auto collapsingNode = mINF.addNode<CollapsingNode>({300, 300});
    }
  }

  void set_size(ImVec2 d) { mINF.setSize(d); }

  void draw() override { mINF.update(); }
};

struct HeronGui : public Application {
  NodeEditor* Editor = nullptr;

  HeronGui(const char* name, int argc, char** argv)
      : Application(name, argc, argv)  // forward to base
  {}

  void Dockspace(const std::function<void()>& render_content) {
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // TODO: remove this later if needed
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;

    if (opt_fullscreen) {
      const ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      window_flags |= ImGuiWindowFlags_NoTitleBar |
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove;
      window_flags |=
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    } else {
      dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
      window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Work Area", nullptr, window_flags);
    if (!opt_padding) ImGui::PopStyleVar();

    if (opt_fullscreen) ImGui::PopStyleVar(2);

    // Dockspace itself
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (render_content) render_content();

    ImGui::End();
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

    Editor = new NodeEditor(500, 500);
  }

  void OnStop() override {
    delete Editor;
    Editor = nullptr;
  }

  void OnFrame(float deltaTime) override {
    Dockspace([&]() {
      ImGui::Begin("Node Editor Demo");

      ImVec2 editor_size = ImGui::GetContentRegionAvail();
      Editor->set_size(editor_size);
      Editor->draw();

      ImGui::End();
      ImGui::ShowDemoWindow();
    });

    ImGui::Begin("Test");
    ImGui::Text("If you see this, we good");
    ImGui::End();
  }
};

int Main(int argc, char** argv) {
  HeronGui example("Blueprints", argc, argv);

  if (example.Create()) return example.Run();

  return 0;
}