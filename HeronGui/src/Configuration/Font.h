#pragma once
#include "imgui.h"

namespace HeronGui::Configuration::Font
{
	// Sans (Google Sans)
	extern ImFont* Italic;
	extern ImFont* Medium;
	extern ImFont* MediumItalic;
	extern ImFont* SemiBold;
	extern ImFont* SemiBoldItalic;
	extern ImFont* Bold;
	extern ImFont* BoldItalic;

	// Code (Google Sans Code)
	extern ImFont* Mono;
	extern ImFont* MonoItalic;
	extern ImFont* MonoMedium;
	extern ImFont* MonoMediumItalic;
	extern ImFont* MonoSemiBold;
	extern ImFont* MonoSemiBoldItalic;
	extern ImFont* MonoBold;
	extern ImFont* MonoBoldItalic;

	void LoadFonts();
}