#pragma once

#include "GameOverlay.h"
#include "Lib/ImGui/imgui.h"
#include "SohConsole.h"

struct GameAsset {
    uint32_t textureId;
    int width;
    int height;
};

namespace SohImGui {
    enum class Backend {
        DX11,
        SDL
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
    } EventImpl;

    extern WindowImpl impl;

    using WindowDrawFunc = void(*)(bool& enabled);

    typedef struct {
        bool enabled;
        WindowDrawFunc drawFunc;
    } CustomWindow;

    extern Console* console;
    extern Ship::GameOverlay* overlay;
    extern bool needs_save;
    void Init(WindowImpl window_impl);
    void Update(EventImpl event);

    void EnhancementRadioButton(const char* text, const char* cvarName, int id);
    void EnhancementCheckbox(const char* text, const char* cvarName);
    void EnhancementButton(const char* text, const char* cvarName);
    void EnhancementSliderInt(const char* text, const char* id, const char* cvarName, int min, int max, const char* format);
    void EnhancementSliderFloat(const char* text, const char* id, const char* cvarName, float min, float max, const char* format, float defaultValue, bool isPercentage);
    
    void Tooltip(const char* text);

    void EnhancementColor3(const char* text, const char* cvarName, float defaultColors[3], bool TitleSameLine);

    void DrawMainMenuAndCalculateGameSize(void);
    
    void DrawFramebufferAndGameInput(void);
    void Render(void);
    void CancelFrame(void);
    void ShowCursor(bool hide, Dialogues w);
    void BindCmd(const std::string& cmd, CommandEntry entry);
    void AddWindow(const std::string& category, const std::string& name, WindowDrawFunc drawFunc);
    void LoadResource(const std::string& name, const std::string& path, const ImVec4& tint = ImVec4(1, 1, 1, 1));
    void LoadInterfaceEditor();
    ImTextureID GetTextureByID(int id);
    ImTextureID GetTextureByName(const std::string& name);
}
