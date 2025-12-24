#pragma once
#include "imgui.h"

// Sans (Google Sans)
extern ImFont* Main;
extern ImFont* Main_Italic;
extern ImFont* Main_Medium;
extern ImFont* Main_MediumItalic;
extern ImFont* Main_SemiBold;
extern ImFont* Main_SemiBoldItalic;
extern ImFont* Main_Bold;
extern ImFont* Main_BoldItalic;

// Code (Google Sans Code)
extern ImFont* Code;
extern ImFont* Code_Italic;
extern ImFont* Code_Medium;
extern ImFont* Code_MediumItalic;
extern ImFont* Code_SemiBold;
extern ImFont* Code_SemiBoldItalic;
extern ImFont* Code_Bold;
extern ImFont* Code_BoldItalic;

namespace HeronGui {
	void SetupImGuiStyle();
    void LoadFonts();
}
