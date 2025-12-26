#include "Font.h"

namespace HeronGui::Configuration::Font
{
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

	void LoadFonts()
	{
		ImGuiIO& io = ImGui::GetIO();
		const float baseSize = 17.0f;

		ImFontConfig cfg;
		cfg.GlyphOffset = ImVec2(0.0f, 0.0f);

		// ---- Google Sans ----
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans/static/GoogleSans-Regular.ttf", baseSize, &cfg);
		Italic = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans/static/GoogleSans-Italic.ttf", baseSize, &cfg);
		Medium = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans/static/GoogleSans-Medium.ttf", baseSize, &cfg);
		MediumItalic = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans/static/GoogleSans-MediumItalic.ttf", baseSize, &cfg);
		SemiBold = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans/static/GoogleSans-SemiBold.ttf", baseSize, &cfg);
		SemiBoldItalic = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans/static/GoogleSans-SemiBoldItalic.ttf", baseSize, &cfg);
		Bold = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans/static/GoogleSans-Bold.ttf", baseSize, &cfg);
		BoldItalic = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans/static/GoogleSans-BoldItalic.ttf", baseSize, &cfg);

		// ---- Google Sans Code (Mono) ----
		Mono = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans_Code/static/GoogleSansCode-Regular.ttf", baseSize, &cfg);
		MonoItalic = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans_Code/static/GoogleSansCode-Italic.ttf", baseSize, &cfg);
		MonoMedium = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans_Code/static/GoogleSansCode-Medium.ttf", baseSize, &cfg);
		MonoMediumItalic = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans_Code/static/GoogleSansCode-MediumItalic.ttf", baseSize, &cfg);
		MonoSemiBold = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans_Code/static/GoogleSansCode-SemiBold.ttf", baseSize, &cfg);
		MonoSemiBoldItalic = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans_Code/static/GoogleSansCode-SemiBoldItalic.ttf", baseSize, &cfg);
		MonoBold = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans_Code/static/GoogleSansCode-Bold.ttf", baseSize, &cfg);
		MonoBoldItalic = io.Fonts->AddFontFromFileTTF("assets/fonts/Google_Sans_Code/static/GoogleSansCode-BoldItalic.ttf", baseSize, &cfg);
	}
}
