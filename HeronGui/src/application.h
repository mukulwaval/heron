#pragma once
    
#include "imgui.h"

#include <functional>

namespace HeronGui {
    void RenderUI();
    void LoadApplicationFonts();
    void SetupImGuiStyle();
    void InitDockspace(const std::function<void()>& renderContent);
}
