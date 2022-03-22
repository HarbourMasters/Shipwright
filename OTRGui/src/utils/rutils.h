#pragma once
#include <string>
#include <vector>
#include "raylib.h"

namespace UIUtils {
	void UIDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color);
	bool GuiIcon(const std::string& icon, const float x, const float y);
	bool GuiIconButton(const std::string& icon, const char* text, const float x, const float y, bool disabled = false, const char* info = NULL);
	void GuiShadowText(const char* text, float x, float y, int size, Color color, Color shadow, float width = 1.0f, bool center = false);
	void GuiToggle(bool* status, const char* text, float x, float y, bool disabled = false, const char* info = NULL);

	inline std::vector<std::string> Split(std::string s, const std::string& delimiter) {
		std::vector<std::string> result;

		size_t pos = 0;
		std::string token;

		while ((pos = s.find(delimiter)) != std::string::npos)
		{
			token = s.substr(0, pos);
			result.push_back(token);
			s.erase(0, pos + delimiter.length());
		}

		if (s.length() != 0)
			result.push_back(s);

		return result;
	}
}
