#pragma once

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

    // Enumeration for disabled checkbox graphics
    enum class ImGuiCheckboxGraphics
    {
        Cross,
        Checkmark,
        None
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


    void Init(WindowImpl window_impl);
    void Update(EventImpl event);

    void DrawMainMenuAndCalculateGameSize(void);
    void RegisterMenuDrawMethod(std::function<void(void)> drawMethod);
    void AddSetupHooksDelegate(std::function<void(void)> setupHooksMethod);

    void DrawFramebufferAndGameInput(void);
    void Render(void);
    void CancelFrame(void);
    void DrawSettings();

    Backend WindowBackend();
    float WindowRefreshRate();

    std::pair<const char*, const char*>* GetAvailableRenderingBackends();
    std::pair<const char*, const char*> GetCurrentRenderingBackend();
    void SetCurrentRenderingBackend(uint8_t index, std::pair<const char*, const char*>);

    const char** GetSupportedTextureFilters();

    void SetResolutionMultiplier(float multiplier);
    void SetMSAALevel(uint32_t value);

    void AddWindow(const std::string& category, const std::string& name, WindowDrawFunc drawFunc, bool isEnabled = false, bool isHidden = false);
    void EnableWindow(const std::string& name, bool isEnabled = true);

    void ToggleInputEditorWindow(bool isOpen = true);
    void ToggleStatisticsWindow(bool isOpen = true);

    void ToggleConsoleWindow(bool isOpen = true);
    void DispatchConsoleCommand(const std::string& line);

    void RequestCvarSaveOnNextTick();

    void LoadResource(const std::string& name, const std::string& path, const ImVec4& tint = ImVec4(1, 1, 1, 1));

    void ShowCursor(bool hide, Dialogues w);
    void BindCmd(const std::string& cmd, Ship::CommandEntry entry);



    ImTextureID GetTextureByID(int id);
    ImTextureID GetTextureByName(const std::string& name);
    void BeginGroupPanel(const char* name, const ImVec2 & size = ImVec2(0.0f, 0.0f));
    void EndGroupPanel(float minHeight = 0.0f);
}

#endif
