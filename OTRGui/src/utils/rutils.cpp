#include "rutils.h"

#include "raylib.h"
#include "game.h"
#define RAYGUI_IMPLEMENTATION
#include "extras/raygui.h"
#include "raymath.h"

namespace UIUtils {
	void UIDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color){
		GuiDrawRectangle(rec, borderWidth, borderColor, color);
	}
	void GuiShadowText(const char* text, float x, float y, int size, Color color, Color shadow, float width, bool center) {
		std::vector<std::string> txt = Split(std::string(text), "\n");
		float height = (txt.size() * 10) / 2.0f;
		for (int nl = 0; nl < txt.size(); nl++) {
			const char* rTxt = txt[nl].c_str();
			const Vector2 textSize = MeasureTextEx(guiFont, rTxt, size, static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SPACING)));
			const Vector2 textPos = Vector2((center ? width / 2.0f - textSize.x / 2.0f : x), y + (nl * 10) - height);
			DrawText(rTxt, textPos.x, textPos.y + 1, size, shadow);
			DrawText(rTxt, textPos.x, textPos.y, size, color);
		}
	}
	void GuiToggle(bool* status, const char* text, float x, float y, bool disabled, const char* info) {
		std::vector<std::string> txt = Split(std::string(text), "\n");
		float height = (txt.size() * 10) / 2.0f;
		const Vector2 boxSize(14, 14);
		const float mouseX = GetMouseX();
		const float mouseY = GetMouseY();
		const bool hovered = mouseX >= x && mouseY >= y && mouseX <= x + boxSize.x && mouseY <= y + boxSize.y && !disabled;
		const Vector2 textSize = MeasureTextEx(guiFont, text, 10, static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SPACING)));
		const Color color = hovered ? YELLOW : WHITE;
		DrawRectangleLines(x, y, boxSize.x, boxSize.y, color);
		if(*status) DrawRectangle(x + 3, y + 3, boxSize.x - 6, boxSize.y - 6, color);
		DrawText(text, x + boxSize.x + 5, y + boxSize.y / 2 - textSize.y / 2, 10, WHITE);
		if(hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) (*status) = !(*status);
	}
	bool GuiIcon(const std::string& icon, const float x, const float y) {
		const Rectangle rect = Rectangle(x, y, 32, 32);
		const float mouseX = GetMouseX();
		const float mouseY = GetMouseY();

		bool hovered = mouseX >= x && mouseY >= y && mouseX <= x + rect.width && mouseY <= y + rect.height;
		DrawTexture(Game->Textures[icon], x, y, WHITE);

		return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	}
	bool GuiIconButton(const std::string& icon, const char* text, const float x, const float y, bool disabled, const char* info) {
		constexpr int iconWidth  = 32;
		constexpr float fontSize = 10;
		const Rectangle rect = Rectangle(x, y, 64, 64);
		const Rectangle infoRect = Rectangle(x + rect.width - 16, y + 4, 12, 12);
		const float mouseX = GetMouseX();
		const float mouseY = GetMouseY();

		bool infoHovered = mouseX >= infoRect.x && mouseY >= infoRect.y && mouseX <= infoRect.x + infoRect.width && mouseY <= infoRect.y + infoRect.height && !disabled;
		bool hovered = mouseX >= x && mouseY >= y && mouseX <= x + rect.width && mouseY <= y + rect.height && !disabled && !infoHovered;

		DrawTexture(Game->Textures["Button"], x, y, hovered ? Color(150, 150, 255, 255) : WHITE);
		DrawTexture(Game->Textures[icon], (rect.x + rect.width / 2.0f - iconWidth / 2.0f), rect.y + 6, WHITE);

		std::vector<std::string> txt = Split(std::string(text), "\n");

		for(int nl = 0; nl < txt.size(); nl++) {
			const char* rTxt = txt[nl].c_str();
			const Vector2 textSize = MeasureTextEx(guiFont, rTxt, fontSize, static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SPACING)));
			const Vector2 textPos = Vector2(rect.x + ((rect.width / 2.0f) - (textSize.x / 2.0f)), rect.y + rect.height / 2 + 8 + ( nl * 10 ));
			DrawText(rTxt, textPos.x, textPos.y + 1, fontSize, BLACK);
			DrawText(rTxt, textPos.x, textPos.y, fontSize, WHITE);
		}

		if (info != nullptr) {
			DrawTexture(Game->Textures["Info"], infoRect.x, infoRect.y, WHITE);
			if (infoHovered) {
				std::vector<std::string> txt = Split(std::string(info), "\n");
				float diff = 0;
				Vector2 textSize(0, 0);
				Vector2 textPos(infoRect.x + infoRect.width + 10, infoRect.y + infoRect.height / 2 - textSize.y / 2);

				for (int nl = 0; nl < txt.size(); nl++) {
					const char* rTxt = txt[nl].c_str();
					Vector2 new_size = MeasureTextEx(guiFont, rTxt, 10, static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SPACING)));
					if (textSize.x < new_size.x) textSize = Vector2(new_size.x, txt.size() * 10);
				}

				DrawRectangle(textPos.x - 5, textPos.y - 5, textSize.x + 10, textSize.y + 10, BLACK);
				DrawRectangleLinesEx(Rectangle(textPos.x - 5, textPos.y - 5, textSize.x + 10, textSize.y + 10), 2, Color(30, 30, 200, 255));
				for (int nl = 0; nl < txt.size(); nl++) {
					const char* rTxt = txt[nl].c_str();
					Vector2 new_size = MeasureTextEx(guiFont, rTxt, 10, static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SPACING)));
					if (nl != 0) textPos = Vector2Add(textPos, Vector2(0, 10));
					if (new_size.x > textSize.x) diff = abs(textSize.x - new_size.x);
					DrawText(rTxt, textPos.x, textPos.y, 10, WHITE);
				}
			}
		}

		return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
	}
}
