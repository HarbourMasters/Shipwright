#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    void enableBetaQuest();
    void disableBetaQuest();
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "GameOverlay.h"
#include "Lib/ImGui/imgui.h"
#include "Console.h"
#include "InputEditor.h"

struct GameAsset {
    uint32_t textureId;
    int width;
    int height;
};

namespace SohImGui {
    enum class Backend {
        DX11,
        SDL,
        GX2,
    };

    enum class Dialogues {
        dConsole,
        dMenubar,
        dLoadSettings,
    };

    typedef struct {
        Backend backend;
        union {
            struct {
                void* window;
                void* device_context;
                void* device;
            } dx11;
            struct {
                void* window;
                void* context;
            } sdl;
            struct {
                uint32_t width;
                uint32_t height;
            } gx2;
        };
    } WindowImpl;

    typedef union {
        struct {
            void* handle;
            int msg;
            int wparam;
            int lparam;
        } win32;
        struct {
            void* event;
        } sdl;
        struct {
            void* input;
        } gx2;
    } EventImpl;

    extern WindowImpl impl;

    using WindowDrawFunc = void(*)(bool& enabled);

    typedef struct {
        bool enabled;
        WindowDrawFunc drawFunc;
    } CustomWindow;

    extern std::shared_ptr<Ship::Console> console;
    extern Ship::InputEditor* controller;
    extern Ship::GameOverlay* overlay;
    extern bool needs_save;
    void Init(WindowImpl window_impl);
    void Update(EventImpl event);
    void Tooltip(const char* text);

    void EnhancementRadioButton(const char* text, const char* cvarName, int id);
    void EnhancementCheckbox(const char* text, const char* cvarName);
    void EnhancementButton(const char* text, const char* cvarName);
    void EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = false);
    void EnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage, bool PlusMinusButton = false);
    void EnhancementCombobox(const char* name, const char* ComboArray[], size_t arraySize, uint8_t FirstTimeValue);
    void EnhancementColor(const char* text, const char* cvarName, ImVec4 ColorRGBA, ImVec4 default_colors, bool allow_rainbow = true, bool has_alpha=false, bool TitleSameLine=false);
    void EnhancementCombo(const std::string& name, const char* cvarName, const std::vector<std::string>& items, int defaultValue = 0);

    void applyEnhancementPresets(void);
    void applyEnhancementPresetDefault(void);
    void applyEnhancementPresetVanillaPlus(void);
    void applyEnhancementPresetEnhanced(void);
    void applyEnhancementPresetRandomizer(void);

    void DrawMainMenuAndCalculateGameSize(void);

    void DrawFramebufferAndGameInput(void);
    void Render(void);
    void CancelFrame(void);
    void ShowCursor(bool hide, Dialogues w);
    void BindCmd(const std::string& cmd, Ship::CommandEntry entry);
    void AddWindow(const std::string& category, const std::string& name, WindowDrawFunc drawFunc, bool isEnabled=false, bool isHidden=false);
    void LoadResource(const std::string& name, const std::string& path, const ImVec4& tint = ImVec4(1, 1, 1, 1));
    void LoadPickersColors(ImVec4& ColorArray, const char* cvarname, const ImVec4& default_colors, bool has_alpha=false);
    int ClampFloatToInt(float value, int min, int max);
    void RandomizeColor(const char* cvarName, ImVec4* colors);
    void RainbowColor(const char* cvarName, ImVec4* colors);
    void ResetColor(const char* cvarName, ImVec4* colors, ImVec4 defaultcolors, bool has_alpha);
    ImTextureID GetTextureByID(int id);
    ImTextureID GetTextureByName(const std::string& name);
    void BeginGroupPanel(const char* name, const ImVec2 & size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel(float minHeight = 0.0f);
    std::string BreakTooltip(const char* text, int lineLength = 60);
    std::string BreakTooltip(const std::string& text, int lineLength = 60);
    void InsertPadding(float extraVerticalPadding = 0.0f);
    void PaddedSeparator(bool padTop = true, bool padBottom = true, float extraVerticalTopPadding = 0.0f, float extraVerticalBottomPadding = 0.0f);
    void PaddedEnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format, int defaultValue = 0, bool PlusMinusButton = false, bool padTop = true, bool padBottom = true);
    void PaddedEnhancementCheckbox(const char* text, const char* cvarName, bool padTop = true, bool padBottom = true);
    void PaddedText(const char* text, bool padTop = true, bool padBottom = true);
    std::string GetWindowButtonText(const char* text, bool menuOpen);
}

#endif
