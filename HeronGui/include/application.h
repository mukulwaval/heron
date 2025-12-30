#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui.h>

#include <memory>
#include <string>

struct Platform;
struct Renderer;

struct Application {
  Application(const char* name);
  Application(const char* name, int argc, char** argv);
  ~Application();

  bool Create(int width = -1, int height = -1);

  int Run();

  void SetTitle(const char* title);

  bool Close();
  void Quit();

  const std::string& GetName() const;

  ImTextureID LoadTexture(const char* path);
  ImTextureID CreateTexture(const void* data, int width, int height);
  void DestroyTexture(ImTextureID texture);
  int GetTextureWidth(ImTextureID texture);
  int GetTextureHeight(ImTextureID texture);

  virtual void OnStart() {}
  virtual void OnStop() {}
  virtual void OnFrame(float deltaTime) {}

  virtual ImGuiWindowFlags GetWindowFlags() const;

  virtual bool CanClose() { return true; }

  ImFont* Italic = nullptr;
  ImFont* Medium = nullptr;
  ImFont* MediumItalic = nullptr;
  ImFont* SemiBold = nullptr;
  ImFont* SemiBoldItalic = nullptr;
  ImFont* Bold = nullptr;
  ImFont* BoldItalic = nullptr;

  ImFont* Mono = nullptr;
  ImFont* MonoItalic = nullptr;
  ImFont* MonoMedium = nullptr;
  ImFont* MonoMediumItalic = nullptr;
  ImFont* MonoSemiBold = nullptr;
  ImFont* MonoSemiBoldItalic = nullptr;
  ImFont* MonoBold = nullptr;
  ImFont* MonoBoldItalic = nullptr;

  std::unique_ptr<Platform> m_Platform;

 private:
  void RecreateFontAtlas();

  void Frame();

  std::string m_Name;
  std::unique_ptr<Renderer> m_Renderer;
  std::string m_IniFilename;
  ImGuiContext* m_Context = nullptr;
};

int Main(int argc, char** argv);