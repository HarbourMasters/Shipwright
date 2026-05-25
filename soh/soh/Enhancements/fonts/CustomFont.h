#pragma once

#include <ship/window/gui/GuiWindow.h>
#include <imgui.h>
#include <vector>
#include <string>

#define CVAR_CUSTOM_FONT_ENABLED      CVAR_ENHANCEMENT("CustomFont.Enabled")
#define CVAR_CUSTOM_FONT_NAME         CVAR_ENHANCEMENT("CustomFont.FontName")
#define CVAR_CUSTOM_FONT_TRANSLATION  CVAR_ENHANCEMENT("CustomFont.Translation")

class CustomFont : public Ship::GuiWindow {
  public:
    using Ship::GuiWindow::GuiWindow;

    // Override Draw entirely: we position the window to match the live textbox REGs every frame.
    void Draw() override;

    // Mod font registry — populated at startup from fonts/*.ttf files in loaded .o2r archives.
    // Call RegisterModFont() once per font during OTRGlobals::StartGame, then GetModFont*() at
    // any time afterward (the registry is stable for the lifetime of the process).
    static void                            RegisterModFont(const std::string& name, ImFont* font);
    static const std::vector<std::string>& GetModFontNames();
    static ImFont*                         GetModFont(const std::string& name);

    static const std::vector<std::string>& GetTranslationNames();
    static void                            LoadTranslation(const std::string& name);

    struct TextSegment {
        std::string text;
        ImVec4      color;
        bool        newline      = false;
        bool        isIcon       = false;   // item icon (MESSAGE_ITEM_ICON)
        uint8_t     itemId       = 0;
        std::string btnIcon;               // non-empty = button/special char icon; value is GUI texture name
        int8_t      choiceIndex  = -1;     // -1 = main text; 0/1/2 = choice option index
        float       shiftX       = 0.0f;   // MESSAGE_SHIFT: pixels to add to current X position
        bool        isAdjustable = false;  // resolve color live from R_TEXT_ADJUST_COLOR_1_R/G/B each frame
        bool        isName       = false;  // MESSAGE_NAME: render the current save file's player name
    };

  protected:
    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {}

  private:
    // Parse msgBufDecoded up to drawLen characters into renderable segments.
    static std::vector<TextSegment> ParseDecodedBuffer(const uint8_t* buf, uint16_t drawLen);

    // Map an OoT color code byte to an ImVec4.
    static ImVec4 ColorFromCode(uint8_t code, const ImVec4& defaultColor);
};
