#pragma once
#include <string>
#include <vector>

#include "Lib/ImGui/imgui.h"
#include <map>
#include <unordered_map>

enum class OverlayType {
	TEXT, IMAGE
};

struct Overlay {
	OverlayType type;
	const char* value;
};

namespace Ship {
	class GameOverlay {
	public:
		std::unordered_map<std::string, Overlay> RegisteredOverlays;
		std::unordered_map<std::string, ImFont*> Fonts;
		std::string CurrentFont = "Default";
		void Init();
		void Draw();
		void DrawSettings();
		static float GetScreenWidth();
		static float GetScreenHeight();
		static float GetStringWidth(const char* text);
		static ImVec2 CalculateTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);
	private:
		void TextDraw(float x, float y, bool shadow, const char* text, ...);
		void LoadFont(const std::string& name, const std::string& path, float fontSize);
	};

	static bool OverlayCommand(const std::vector<std::string>& args);
}

