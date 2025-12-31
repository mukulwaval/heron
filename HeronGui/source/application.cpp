#include "application.h"

// Standard includes
#include <algorithm>  // For std::find_if
#include <cstdint>    // For std::uintptr_t

// ImGui
#include "imgui.h"
#include "imgui_impl_opengl3.h"

// OpenGL loader (pick one based on your setup)
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
using namespace gl;
#else
#include "imgui_impl_opengl3_loader.h"
#endif

#include "platform.h"
#include "renderer.h"
#include "setup.h"

extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC

#include "stb_image.h"
}

Application::Application(const char* name) : Application(name, 0, nullptr) {}

Application::Application(const char* name, int argc, char** argv)
    : m_Name(name),
      m_Platform(CreatePlatform(*this)),
      m_Renderer(CreateRenderer()) {
  m_Platform->ApplicationStart(argc, argv);
}

Application::~Application() {
  m_Renderer->Destroy();

  m_Platform->ApplicationStop();

  if (m_Context) {
    ImGui::DestroyContext(m_Context);
    m_Context = nullptr;
  }
}

bool Application::Create(int width /*= -1*/, int height /*= -1*/) {
  m_Context = ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui::SetCurrentContext(m_Context);

  if (!m_Platform->OpenMainWindow("Application", width, height)) return false;

  if (!m_Renderer->Create(*m_Platform)) return false;

  m_IniFilename = m_Name + ".ini";

  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
  // Controls
  io.IniFilename = m_IniFilename.c_str();
  io.LogFilename = nullptr;

  ImGui::StyleColorsDark();

  RecreateFontAtlas();

  m_Platform->AcknowledgeWindowScaleChanged();
  m_Platform->AcknowledgeFramebufferScaleChanged();

  OnStart();

  Frame();

  return true;
}

int Application::Run() {
  m_Platform->ShowMainWindow();

  while (m_Platform->ProcessMainWindowEvents()) {
    if (!m_Platform->IsMainWindowVisible()) continue;

    Frame();
  }

  OnStop();

  return 0;
}

void Application::RecreateFontAtlas() {}

void Application::Frame() {
  if (m_Platform->HasWindowScaleChanged())
    m_Platform->AcknowledgeWindowScaleChanged();
  if (m_Platform->HasFramebufferScaleChanged()) {
    RecreateFontAtlas();
    m_Platform->AcknowledgeFramebufferScaleChanged();
  }

  const float windowScale = m_Platform->GetWindowScale();
  const float framebufferScale = m_Platform->GetFramebufferScale();

  ImGuiIO& io = ImGui::GetIO();
  if (io.WantSetMousePos) {
    io.MousePos.x *= windowScale;
    io.MousePos.y *= windowScale;
  }

  m_Platform->NewFrame();
  m_Renderer->NewFrame();
  ImGui::NewFrame();

  OnFrame(io.DeltaTime);

  ImGui::Render();

  // Clear main window framebuffer
  m_Renderer->Clear(ImColor(30, 30, 30, 255));
  m_Renderer->RenderDrawData(ImGui::GetDrawData());

#if PLATFORM(WINDOWS)
  //renderer handles backend nonsense
  m_Renderer->BeginImGuiPlatformWindows();
  ImGui::UpdatePlatformWindows();
  ImGui::RenderPlatformWindowsDefault();
  m_Renderer->EndImGuiPlatformWindows();
#endif

  m_Platform->FinishFrame();
}

void Application::SetTitle(const char* title) const {
  m_Platform->SetMainWindowTitle(title);
}

bool Application::Close() const { return m_Platform->CloseMainWindow(); }

void Application::Quit() const { m_Platform->Quit(); }

const std::string& Application::GetName() const { return m_Name; }

ImTextureID Application::LoadTexture(const char* path) {
  int width = 0, height = 0, component = 0;
  if (auto data = stbi_load(path, &width, &height, &component, 4)) {
    auto texture = CreateTexture(data, width, height);
    stbi_image_free(data);
    return texture;
  } else
    return (ImTextureID) nullptr;
}

ImTextureID Application::CreateTexture(const void* data, int width,
                                       int height) {
  return m_Renderer->CreateTexture(data, width, height);
}

void Application::DestroyTexture(ImTextureID texture) {
  m_Renderer->DestroyTexture(texture);
}

int Application::GetTextureWidth(ImTextureID texture) {
  return m_Renderer->GetTextureWidth(texture);
}

int Application::GetTextureHeight(ImTextureID texture) {
  return m_Renderer->GetTextureHeight(texture);
}

ImGuiWindowFlags Application::GetWindowFlags() const {
  return ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
         /*ImGuiWindowFlags_NoMove |*/ ImGuiWindowFlags_NoScrollbar |
         ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
         ImGuiWindowFlags_NoBringToFrontOnFocus;
}
