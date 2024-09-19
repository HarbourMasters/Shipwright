#include "SohMenuBar.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include "regex"
#include "public/bridge/consolevariablebridge.h"
#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "include/z64audio.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "OTRGlobals.h"
#include "z64.h"
#include "Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/presets.h"
#include "soh/Enhancements/mods.h"
#include "Enhancements/cosmetics/authenticGfxPatches.h"
#ifdef ENABLE_REMOTE_CONTROL
#include "Enhancements/crowd-control/CrowdControl.h"
#include "Enhancements/game-interactor/GameInteractor_Sail.h"
#endif


#include "Enhancements/audio/AudioEditor.h"
#include "Enhancements/controls/InputViewer.h"
#include "Enhancements/cosmetics/CosmeticsEditor.h"
#include "Enhancements/debugger/actorViewer.h"
#include "Enhancements/debugger/colViewer.h"
#include "Enhancements/debugger/debugSaveEditor.h"
#include "Enhancements/debugger/dlViewer.h"
#include "Enhancements/debugger/valueViewer.h"
#include "Enhancements/gameplaystatswindow.h"
#include "Enhancements/debugger/MessageViewer.h"
#include "Enhancements/randomizer/randomizer_check_tracker.h"
#include "Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "Enhancements/randomizer/randomizer_item_tracker.h"
#include "Enhancements/randomizer/randomizer_settings_window.h"
#include "Enhancements/resolution-editor/ResolutionEditor.h"

extern bool isBetaQuestEnabled;

extern "C" PlayState* gPlayState;

std::string GetWindowButtonText(const char* text, bool menuOpen) {
    char buttonText[100] = "";
    if (menuOpen) {
        strcat(buttonText, ICON_FA_CHEVRON_RIGHT " ");
    }
    strcat(buttonText, text);
    if (!menuOpen) { strcat(buttonText, "  "); }
    return buttonText;
}

static std::unordered_map<Ship::WindowBackend, const char*> windowBackendNames = {
    { Ship::WindowBackend::FAST3D_DXGI_DX11, "DirectX" },
    { Ship::WindowBackend::FAST3D_SDL_OPENGL, "OpenGL" },
    { Ship::WindowBackend::FAST3D_SDL_METAL, "Metal" },
};

static const char* imguiScaleOptions[4] = { "Small", "Normal", "Large", "X-Large" };

    static const char* filters[3] = {
#ifdef __WIIU__
            "",
#else
            "Three-Point",
#endif
            "Linear", "None"
    };

    static const char* chestStyleMatchesContentsOptions[4] = { "Disabled", "Both", "Texture Only", "Size Only" };
    static const char* bunnyHoodOptions[3] = { "Disabled", "Faster Run & Longer Jump", "Faster Run" };
    static const char* mirroredWorldModes[9] = {
        "Disabled",           "Always",        "Random",          "Random (Seeded)",          "Dungeons",
        "Dungeons (Vanilla)", "Dungeons (MQ)", "Dungeons Random", "Dungeons Random (Seeded)",
    };
    static const char* enemyRandomizerModes[3] = { "Disabled", "Random", "Random (Seeded)" };
    static const char* allPowers[9] = {
                        "Vanilla (1x)",
                        "Double (2x)",
                        "Quadruple (4x)",
                        "Octuple (8x)",
                        "Foolish (16x)",
                        "Ridiculous (32x)",
                        "Merciless (64x)",
                        "Pure Torture (128x)",
                        "OHKO (256x)" };
    static const char* subPowers[8] = { allPowers[0], allPowers[1], allPowers[2], allPowers[3], allPowers[4], allPowers[5], allPowers[6], allPowers[7] };
    static const char* subSubPowers[7] = { allPowers[0], allPowers[1], allPowers[2], allPowers[3], allPowers[4], allPowers[5], allPowers[6] };
    static const char* zFightingOptions[3] = { "Disabled", "Consistent Vanish", "No Vanish" };
    static const char* autosaveLabels[6] = { "Off", "New Location + Major Item", "New Location + Any Item", "New Location", "Major Item", "Any Item" };
    static const char* DebugSaveFileModes[3] = { "Off", "Vanilla", "Maxed" };
    static const char* FastFileSelect[5] = { "File N.1", "File N.2", "File N.3", "Zelda Map Select (require OoT Debug Mode)", "File select" };
    static const char* DekuStickCheat[3] = { "Normal", "Unbreakable", "Unbreakable + Always on Fire" };
    static const char* bonkDamageValues[8] = {
        "No Damage",
        "0.25 Heart",
        "0.5 Heart",
        "1 Heart",
        "2 Hearts",
        "4 Hearts",
        "8 Hearts",
        "OHKO"
    };
    static const char* timeTravelOptions[3] = { "Disabled", "Ocarina of Time", "Any Ocarina" };
    static const char* swordToggleModes[3] = { "Disabled", "Child Toggle", "Both Ages (May lead to unintended behaviour)"};

extern "C" SaveContext gSaveContext;

namespace SohGui {

std::unordered_map<Ship::WindowBackend, const char*> availableWindowBackendsMap;
Ship::WindowBackend configWindowBackend;

void UpdateWindowBackendObjects() {
    Ship::WindowBackend runningWindowBackend = Ship::Context::GetInstance()->GetWindow()->GetWindowBackend();
    int32_t configWindowBackendId = Ship::Context::GetInstance()->GetConfig()->GetInt("Window.Backend.Id", -1);
    if (Ship::Context::GetInstance()->GetWindow()->IsAvailableWindowBackend(configWindowBackendId)) {
        configWindowBackend = static_cast<Ship::WindowBackend>(configWindowBackendId);
    } else {
        configWindowBackend = runningWindowBackend;
    }

    auto availableWindowBackends = Ship::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends();
    for (auto& backend : *availableWindowBackends) {
        availableWindowBackendsMap[backend] = windowBackendNames[backend];
    }
}

void DrawMenuBarIcon() {
    static bool gameIconLoaded = false;
    if (!gameIconLoaded) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadTextureFromRawImage("Game_Icon", "textures/icons/gIcon.png");
        gameIconLoaded = true;
    }

    if (Ship::Context::GetInstance()->GetWindow()->GetGui()->HasTextureByName("Game_Icon")) {
#ifdef __SWITCH__
        ImVec2 iconSize = ImVec2(20.0f, 20.0f);
        float posScale = 1.0f;
#elif defined(__WIIU__)
        ImVec2 iconSize = ImVec2(16.0f * 2, 16.0f * 2);
        float posScale = 2.0f;
#else
        ImVec2 iconSize = ImVec2(16.0f, 16.0f);
        float posScale = 1.0f;
#endif
        ImGui::SetCursorPos(ImVec2(5, 2.5f) * posScale);
        ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Game_Icon"), iconSize);
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(25, 0) * posScale);
    }
}

void DrawShipMenu() {
    if (ImGui::BeginMenu("Ship")) {
        if (ImGui::MenuItem("Hide Menu Bar",
#if !defined(__SWITCH__) && !defined(__WIIU__)
         "F1"
#else
         "[-]"
#endif
        )) {
            Ship::Context::GetInstance()->GetWindow()->GetGui()->GetMenuBar()->ToggleVisibility();
        }
        UIWidgets::Spacer(0);
#if !defined(__SWITCH__) && !defined(__WIIU__)
        if (ImGui::MenuItem("Toggle Fullscreen", "F11")) {
            Ship::Context::GetInstance()->GetWindow()->ToggleFullscreen();
        }
        UIWidgets::Spacer(0);
#endif
        if (ImGui::MenuItem("Reset",
#ifdef __APPLE__
                            "Command-R"
#elif !defined(__SWITCH__) && !defined(__WIIU__)
                            "Ctrl+R"
#else
                            ""
#endif
                            )) {
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
        }
#if !defined(__SWITCH__) && !defined(__WIIU__)
        UIWidgets::Spacer(0);
        if (ImGui::MenuItem("Open App Files Folder")) {
            std::string filesPath = Ship::Context::GetInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        }
        UIWidgets::Spacer(0);

        if (ImGui::MenuItem("Quit")) {
            Ship::Context::GetInstance()->GetWindow()->Close();
        }
#endif
        ImGui::EndMenu();
    }
}

extern std::shared_ptr<Ship::GuiWindow> mInputEditorWindow;
extern std::shared_ptr<InputViewer> mInputViewer;
extern std::shared_ptr<InputViewerSettingsWindow> mInputViewerSettings;
extern std::shared_ptr<AdvancedResolutionSettings::AdvancedResolutionSettingsWindow> mAdvancedResolutionSettingsWindow;

void DrawSettingsMenu() {
    if (ImGui::BeginMenu("Settings"))
    {
        if (ImGui::BeginMenu("Audio")) {
            UIWidgets::PaddedEnhancementSliderFloat("Master Volume: %.1f %%", "##Master_Vol", CVAR_SETTING("Volume.Master"), 0.0f, 1.0f, "", 1.0f, true, true, false, true);
            if (UIWidgets::PaddedEnhancementSliderFloat("Main Music Volume: %.1f %%", "##Main_Music_Vol", CVAR_SETTING("Volume.MainMusic"), 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_PLAYER_BGM_MAIN, CVarGetFloat(CVAR_SETTING("Volume.MainMusic"), 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Sub Music Volume: %.1f %%", "##Sub_Music_Vol", CVAR_SETTING("Volume.SubMusic"), 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_PLAYER_BGM_SUB, CVarGetFloat(CVAR_SETTING("Volume.SubMusic"), 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Sound Effects Volume: %.1f %%", "##Sound_Effect_Vol", CVAR_SETTING("Volume.SFX"), 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_PLAYER_SFX, CVarGetFloat(CVAR_SETTING("Volume.SFX"), 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Fanfare Volume: %.1f %%", "##Fanfare_Vol", CVAR_SETTING("Volume.Fanfare"), 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_PLAYER_FANFARE, CVarGetFloat(CVAR_SETTING("Volume.Fanfare"), 1.0f));
            }

            static std::unordered_map<Ship::AudioBackend, const char*> audioBackendNames = {
                { Ship::AudioBackend::WASAPI, "Windows Audio Session API" },
                { Ship::AudioBackend::SDL, "SDL" }
            };

            ImGui::Text("Audio API (Needs reload)");
            auto currentAudioBackend = Ship::Context::GetInstance()->GetAudio()->GetAudioBackend();

            if (Ship::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::BeginCombo("##AApi", audioBackendNames[currentAudioBackend])) {
                for (uint8_t i = 0; i < Ship::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size(); i++) {
                    auto backend = Ship::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->data()[i];
                    if (ImGui::Selectable(audioBackendNames[backend], backend == currentAudioBackend)) {
                        Ship::Context::GetInstance()->GetAudio()->SetAudioBackend(backend);
                    }
                }
                ImGui::EndCombo();
            }
            if (Ship::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1) {
                UIWidgets::ReEnableComponent("");
            }

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Controller")) {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 (12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            if (mInputEditorWindow) {
                if (ImGui::Button(GetWindowButtonText("Controller Mapping", CVarGetInteger(CVAR_CONTROLLER_CONFIGURATION_WINDOW_OPEN, 0)).c_str(), ImVec2 (-1.0f, 0.0f))) {
                    mInputEditorWindow->ToggleVisibility();
                }
            }
            UIWidgets::PaddedSeparator();
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(3);
        #ifndef __SWITCH__
            UIWidgets::EnhancementCheckbox("Menubar Controller Navigation", CVAR_IMGUI_CONTROLLER_NAV);
            UIWidgets::Tooltip("Allows controller navigation of the SOH menu bar (Settings, Enhancements,...)\nCAUTION: This will disable game inputs while the menubar is visible.\n\nD-pad to move between items, A to select, and X to grab focus on the menu bar");
            UIWidgets::PaddedSeparator();
        #endif
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 (12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            if (mInputViewer) {
                if (ImGui::Button(GetWindowButtonText("Input Viewer", CVarGetInteger(CVAR_WINDOW("InputViewer"), 0)).c_str(), ImVec2 (-1.0f, 0.0f))) {
                    mInputViewer->ToggleVisibility();
                }
            }
            if (mInputViewerSettings) {
                if (ImGui::Button(GetWindowButtonText("Input Viewer Settings", CVarGetInteger(CVAR_WINDOW("InputViewerSettings"), 0)).c_str(), ImVec2 (-1.0f, 0.0f))) {
                    mInputViewerSettings->ToggleVisibility();
                }
            }
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(3);

            UIWidgets::PaddedEnhancementSliderInt("Simulated Input Lag: %d frames", "##SimulatedInputLag", CVAR_SIMULATED_INPUT_LAG, 0, 6, "", 0, true, true, false);
            UIWidgets::Tooltip("Buffers your inputs to be executed a specified amount of frames later");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Graphics")) {
        #ifndef __APPLE__
            const bool disabled_resolutionSlider = CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".VerticalResolutionToggle", 0) &&
                                                   CVarGetInteger(CVAR_PREFIX_ADVANCED_RESOLUTION ".Enabled", 0);
            if (UIWidgets::EnhancementSliderFloat("Internal Resolution: %.1f %%", "##IMul", CVAR_INTERNAL_RESOLUTION, 0.5f,
                                                  2.0f, "", 1.0f, true, true, disabled_resolutionSlider)) {
                Ship::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(CVarGetFloat(CVAR_INTERNAL_RESOLUTION, 1));
            }
            UIWidgets::Tooltip("Resolution scale. Multiplies output resolution by this value, on each axis relative to window size.\n"
                               "Lower values may improve performance.\n"
                               "Values above 100% can be used for super-sampling, as an intensive but highly effective form of anti-aliasing.\n\n"
                               "Default: 100%");
            
            if (mAdvancedResolutionSettingsWindow) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                UIWidgets::Spacer(0);
                if (ImGui::Button(GetWindowButtonText("Advanced Resolution", CVarGetInteger(CVAR_WINDOW("AdvancedResolutionEditor"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                    mAdvancedResolutionSettingsWindow->ToggleVisibility();
                }
                ImGui::PopStyleColor(1);
                ImGui::PopStyleVar(3);
            }
        #else
            // macOS: Internal resolution is currently disabled in libultraship.
            ImGui::BeginGroup();
            ImGui::Text("Internal Resolution: 100.0%%");
            UIWidgets::Spacer(0);
            ImGui::Text(" " ICON_FA_INFO_CIRCLE " Not available on this system.");
            UIWidgets::Spacer(0);
            ImGui::EndGroup();
        #endif

        #ifndef __WIIU__
            if (UIWidgets::PaddedEnhancementSliderInt(
                    (CVarGetInteger(CVAR_MSAA_VALUE, 1) == 1) ? "Anti-aliasing (MSAA): Off" : "Anti-aliasing (MSAA): %d",
                    "##IMSAA", CVAR_MSAA_VALUE, 1, 8, "", 1, true, true, false)) {
                Ship::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger(CVAR_MSAA_VALUE, 1));
            }
            UIWidgets::Tooltip("Activates MSAA (multi-sample anti-aliasing) from 2x up to 8x, to smooth the edges of rendered geometry.\n"
                               "Higher sample count will result in smoother edges on models, but may reduce performance.\n\n"
                               "Recommended: 2x or 4x");
        #endif

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
            { // FPS Slider
                const int minFps = 20;
                static int maxFps;
                if (Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() == Ship::WindowBackend::FAST3D_DXGI_DX11) {
                    maxFps = 360;
                } else {
                    maxFps = Ship::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
                }
                int currentFps = fmax(fmin(OTRGlobals::Instance->GetInterpolationFPS(), maxFps), minFps);
            #ifdef __WIIU__
                UIWidgets::Spacer(0);
                // only support divisors of 60 on the Wii U
                if (currentFps > 60) {
                    currentFps = 60;
                } else {
                    currentFps = 60 / (60 / currentFps);
                }

                int fpsSlider = 1;
                if (currentFps == 20) {
                    ImGui::Text("FPS: Original (20)");
                } else {
                    ImGui::Text("FPS: %d", currentFps);
                    if (currentFps == 30) {
                        fpsSlider = 2;
                    } else { // currentFps == 60
                        fpsSlider = 3;
                    }
                }
                if (CVarGetInteger(CVAR_SETTING("MatchRefreshRate"), 0)) {
                    UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
                }

                if (ImGui::Button(" - ##WiiUFPS")) {
                    fpsSlider--;
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

                UIWidgets::Spacer(0);

                ImGui::PushItemWidth(std::min((ImGui::GetContentRegionAvail().x - 60.0f), 260.0f));
                ImGui::SliderInt("##WiiUFPSSlider", &fpsSlider, 1, 3, "", ImGuiSliderFlags_AlwaysClamp);
                ImGui::PopItemWidth();

                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                if (ImGui::Button(" + ##WiiUFPS")) {
                    fpsSlider++;
                }

                if (CVarGetInteger(CVAR_SETTING("MatchRefreshRate"), 0)) {
                    UIWidgets::ReEnableComponent("");
                }
                if (fpsSlider > 3) {
                    fpsSlider = 3;
                } else if (fpsSlider < 1) {
                    fpsSlider = 1;
                }

                if (fpsSlider == 1) {
                    currentFps = 20;
                } else if (fpsSlider == 2) {
                    currentFps = 30;
                } else if (fpsSlider == 3) {
                    currentFps = 60;
                }
                CVarSetInteger(CVAR_SETTING("InterpolationFPS"), currentFps);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            #else
                bool matchingRefreshRate =
                    CVarGetInteger(CVAR_SETTING("MatchRefreshRate"), 0) && Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() != Ship::WindowBackend::FAST3D_DXGI_DX11;
                UIWidgets::PaddedEnhancementSliderInt(
                    (currentFps == 20) ? "Frame Rate: Original (20 fps)" : "Frame Rate: %d fps",
                    "##FPSInterpolation", CVAR_SETTING("InterpolationFPS"), minFps, maxFps, "", 20, true, true, false, matchingRefreshRate);
            #endif
                if (Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() == Ship::WindowBackend::FAST3D_DXGI_DX11) {
                    UIWidgets::Tooltip(
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics.\n"
                        "This is purely visual and does not impact game logic, execution of glitches etc.\n"
                        "Higher frame rate settings may impact CPU performance."
                        "\n\n " ICON_FA_INFO_CIRCLE 
                        " There is no need to set this above your monitor's refresh rate. Doing so will waste resources and may give a worse result.");
                } else {
                    UIWidgets::Tooltip(
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics.\n"
                        "This is purely visual and does not impact game logic, execution of glitches etc.\n"
                        "Higher frame rate settings may impact CPU performance.");
                }
            } // END FPS Slider

            if (Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() == Ship::WindowBackend::FAST3D_DXGI_DX11) {
                UIWidgets::Spacer(0);
                if (ImGui::Button("Match Frame Rate to Refresh Rate")) {
                    int hz = Ship::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
                    if (hz >= 20 && hz <= 360) {
                        CVarSetInteger(CVAR_SETTING("InterpolationFPS"), hz);
                        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                }
            } else {
                UIWidgets::PaddedEnhancementCheckbox("Match Frame Rate to Refresh Rate", CVAR_SETTING("MatchRefreshRate"), true, false);
            }
            UIWidgets::Tooltip("Matches interpolation value to the game window's current refresh rate.");

            if (Ship::Context::GetInstance()->GetWindow()->GetWindowBackend() == Ship::WindowBackend::FAST3D_DXGI_DX11) {
                UIWidgets::PaddedEnhancementSliderInt(CVarGetInteger(CVAR_SETTING("ExtraLatencyThreshold"), 80) == 0 ? "Jitter fix: Off" : "Jitter fix: >= %d FPS",
                    "##ExtraLatencyThreshold", CVAR_SETTING("ExtraLatencyThreshold"), 0, 360, "", 80, true, true, false);
                UIWidgets::Tooltip(
                    "(For DirectX backend only)\n\n"
                    "When Interpolation FPS (Frame Rate) setting is at least this threshold, add one frame of delay (e.g. 16.6 ms for 60 FPS) in order to avoid jitter."
                    "This setting allows the CPU to work on one frame while GPU works on the previous frame.\n"
                    "This setting should be used when your computer is too slow to do CPU + GPU work in time.");
            }

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);

            ImGui::Text("ImGui Menu Scale");
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, "(Experimental)");
            if (UIWidgets::EnhancementCombobox(CVAR_SETTING("ImGuiScale"), imguiScaleOptions, 1)) {
                OTRGlobals::Instance->ScaleImGui();
            }
            UIWidgets::Tooltip("Changes the scaling of the ImGui menu elements.");

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);

            ImGui::Text("Renderer API (Needs reload)");

            if (availableWindowBackendsMap.size() <= 1) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::BeginCombo("##RApi", availableWindowBackendsMap[configWindowBackend])) {
                for (auto backend : availableWindowBackendsMap) {
                    if (ImGui::Selectable(backend.second, backend.first == configWindowBackend)) {
                        Ship::Context::GetInstance()->GetConfig()->SetInt("Window.Backend.Id", static_cast<int>(backend.first));
                        Ship::Context::GetInstance()->GetConfig()->SetString("Window.Backend.Name", backend.second);
                        Ship::Context::GetInstance()->GetConfig()->Save();
                        UpdateWindowBackendObjects();
                    }
                }
                ImGui::EndCombo();
            }
            if (availableWindowBackendsMap.size() <= 1) {
                UIWidgets::ReEnableComponent("");
            }

            if (Ship::Context::GetInstance()->GetWindow()->CanDisableVerticalSync()) {
                UIWidgets::PaddedEnhancementCheckbox("Enable Vsync", CVAR_VSYNC_ENABLED, true, false);
                UIWidgets::Tooltip("Activate vertical sync, to prevent screen tearing.");
            }

            if (Ship::Context::GetInstance()->GetWindow()->SupportsWindowedFullscreen()) {
                UIWidgets::PaddedEnhancementCheckbox("Windowed fullscreen", CVAR_SDL_WINDOWED_FULLSCREEN, true, false);
            }

            if (Ship::Context::GetInstance()->GetWindow()->GetGui()->SupportsViewports()) {
                UIWidgets::PaddedEnhancementCheckbox("Allow multi-windows (Needs reload)", CVAR_ENABLE_MULTI_VIEWPORTS, true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
                UIWidgets::Tooltip("Allows windows to be able to be dragged off of the main game window. Requires a reload to take effect.");
            }

            // If more filters are added to LUS, make sure to add them to the filters list here
            ImGui::Text("Texture Filtering (Needs reload)");
            UIWidgets::EnhancementCombobox(CVAR_TEXTURE_FILTER, filters, FILTER_THREE_POINT);
            UIWidgets::Tooltip("Texture filtering, aka texture smoothing. Requires a reload to take effect.\n\n"
                               "Three-Point: Replicates real N64 texture filtering.\n"
                               "Bilinear: If Three-Point causes poor performance, try this.\n"
                               "Nearest: Disables texture smoothing. (Not recommended)");

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);

            // Draw LUS settings menu (such as Overlays Text Font)
            Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->DrawSettings();

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Languages")) {
            UIWidgets::PaddedEnhancementCheckbox("Translate Title Screen", CVAR_SETTING("TitleScreenTranslation"));
            if (UIWidgets::EnhancementRadioButton("English", CVAR_SETTING("Languages"), LANGUAGE_ENG)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("German", CVAR_SETTING("Languages"), LANGUAGE_GER)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("French", CVAR_SETTING("Languages"), LANGUAGE_FRA)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            ImGui::EndMenu();
        }
        
        UIWidgets::Spacer(0);
        
        if (ImGui::BeginMenu("Accessibility")) {
        #if defined(_WIN32) || defined(__APPLE__)
            UIWidgets::PaddedEnhancementCheckbox("Text to Speech", CVAR_SETTING("A11yTTS"));
            UIWidgets::Tooltip("Enables text to speech for in game dialog");
        #endif
            UIWidgets::PaddedEnhancementCheckbox("Disable Idle Camera Re-Centering", CVAR_SETTING("A11yDisableIdleCam"));
            UIWidgets::Tooltip("Disables the automatic re-centering of the camera when idle.");
            
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

extern std::shared_ptr<AudioEditor> mAudioEditorWindow;
extern std::shared_ptr<CosmeticsEditorWindow> mCosmeticsEditorWindow;
extern std::shared_ptr<GameplayStatsWindow> mGameplayStatsWindow;

void DrawEnhancementsMenu() {
    if (ImGui::BeginMenu("Enhancements"))
    {
        ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));

        DrawPresetSelector(PRESET_TYPE_ENHANCEMENTS);

        UIWidgets::PaddedSeparator();

        if (ImGui::BeginMenu("Gameplay"))
        {
            if (ImGui::BeginMenu("Time Savers"))
            {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
                ImGui::BeginTable("##timeSaversMenu", 2, ImGuiTableFlags_SizingFixedFit);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableNextColumn();
                UIWidgets::Spacer(0);
                ImGui::Text("Speed-ups:");
                UIWidgets::PaddedSeparator();

                UIWidgets::PaddedEnhancementSliderInt("Text Speed: %dx", "##TEXTSPEED", CVAR_ENHANCEMENT("TextSpeed"), 1, 5, "", 1, true, false, true);
                UIWidgets::PaddedEnhancementCheckbox("Skip Text", CVAR_ENHANCEMENT("SkipText"), false, true);
                UIWidgets::Tooltip("Holding down B skips text");
                UIWidgets::PaddedEnhancementSliderInt("King Zora Speed: %dx", "##MWEEPSPEED", CVAR_ENHANCEMENT("MweepSpeed"), 1, 5, "", 1, true, false, true);
                UIWidgets::PaddedEnhancementSliderInt("Vine/Ladder Climb speed +%d", "##CLIMBSPEED", CVAR_ENHANCEMENT("ClimbSpeed"), 0, 12, "", 0, true, false, true);
                UIWidgets::PaddedEnhancementSliderInt("Block pushing speed +%d", "##BLOCKSPEED", CVAR_ENHANCEMENT("FasterBlockPush"), 0, 5, "", 0, true, false, true);
                UIWidgets::PaddedEnhancementSliderInt("Crawl speed %dx", "##CRAWLSPEED", CVAR_ENHANCEMENT("CrawlSpeed"), 1, 4, "", 1, true, false, true);
                UIWidgets::PaddedEnhancementCheckbox("Faster Heavy Block Lift", CVAR_ENHANCEMENT("FasterHeavyBlockLift"), false, false);
                UIWidgets::Tooltip("Speeds up lifting silver rocks and obelisks");
                UIWidgets::PaddedEnhancementCheckbox("Skip Pickup Messages", CVAR_ENHANCEMENT("FastDrops"), true, false);
                UIWidgets::Tooltip("Skip pickup messages for new consumable items and bottle swipes");
                UIWidgets::PaddedEnhancementCheckbox("Fast Ocarina Playback", CVAR_ENHANCEMENT("FastOcarinaPlayback"), true, false);
                UIWidgets::Tooltip("Skip the part where the Ocarina playback is called when you play a song");
                bool forceSkipScarecrow = IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SKIP_SCARECROWS_SONG);
                static const char* forceSkipScarecrowText = "This setting is forcefully enabled because a savefile\nwith \"Skip Scarecrow Song\" is loaded";
                UIWidgets::PaddedEnhancementCheckbox("Skip Scarecrow Song", CVAR_ENHANCEMENT("InstantScarecrow"), true, false,
                                                        forceSkipScarecrow, forceSkipScarecrowText, UIWidgets::CheckboxGraphics::Checkmark);
                UIWidgets::Tooltip("Pierre appears when Ocarina is pulled out. Requires learning scarecrow song.");
                UIWidgets::PaddedEnhancementCheckbox("Skip Magic Arrow Equip Animation", CVAR_ENHANCEMENT("SkipArrowAnimation"), true, false);
                UIWidgets::PaddedEnhancementCheckbox("Skip save confirmation", CVAR_ENHANCEMENT("SkipSaveConfirmation"), true, false);
                UIWidgets::Tooltip("Skip the \"Game saved.\" confirmation screen");
                UIWidgets::PaddedEnhancementCheckbox("Faster Farore's Wind", CVAR_ENHANCEMENT("FastFarores"), true, false);
                UIWidgets::Tooltip("Greatly decreases cast time of Farore's Wind magic spell.");
                UIWidgets::PaddedEnhancementCheckbox("Skip water take breath animation", CVAR_ENHANCEMENT("SkipSwimDeepEndAnim"), true, false);
                UIWidgets::Tooltip("Skips Link's taking breath animation after coming up from water. This setting does not interfere with getting items from underwater.");

                ImGui::TableNextColumn();
                UIWidgets::Spacer(0);
                ImGui::Text("Changes:");
                UIWidgets::PaddedSeparator();
                
                UIWidgets::PaddedEnhancementSliderInt("Biggoron Forge Time: %d days", "##FORGETIME", CVAR_ENHANCEMENT("ForgeTime"), 0, 3, "", 3, true, false, true);
                UIWidgets::Tooltip("Allows you to change the number of days it takes for Biggoron to forge the Biggoron Sword");
                UIWidgets::PaddedEnhancementCheckbox("Remember Save Location", CVAR_ENHANCEMENT("RememberSaveLocation"), false, false);
                UIWidgets::Tooltip("When loading a save, places Link at the last entrance he went through.\n"
                        "This doesn't work if the save was made in grottos/fairy fountains or dungeons.");
                UIWidgets::PaddedEnhancementCheckbox("No Forced Navi", CVAR_ENHANCEMENT("NoForcedNavi"), true, false);
                UIWidgets::Tooltip("Prevent forced Navi conversations");
                UIWidgets::PaddedEnhancementCheckbox("Navi Timer Resets", CVAR_ENHANCEMENT("ResetNaviTimer"), true, false);
                UIWidgets::Tooltip("Resets the Navi timer on scene change. If you have already talked to her, she will try and talk to you again, instead of needing a save warp or death. ");
                UIWidgets::PaddedEnhancementCheckbox("No Skulltula Freeze", CVAR_ENHANCEMENT("SkulltulaFreeze"), true, false);
                UIWidgets::Tooltip("Stops the game from freezing the player when picking up Gold Skulltulas");
                UIWidgets::PaddedEnhancementCheckbox("Nighttime GS Always Spawn", CVAR_ENHANCEMENT("NightGSAlwaysSpawn"), true, false);
                UIWidgets::Tooltip("Nighttime Skulltulas will spawn during both day and night.");
                UIWidgets::PaddedEnhancementCheckbox("Dampe Appears All Night", CVAR_ENHANCEMENT("DampeAllNight"), true, false);
                UIWidgets::Tooltip("Makes Dampe appear anytime during the night, not just his usual working hours.");
                UIWidgets::PaddedEnhancementCheckbox("Fast Chests", CVAR_ENHANCEMENT("FastChests"), true, false);
                UIWidgets::Tooltip("Kick open every chest");
                UIWidgets::PaddedText("Chest size & texture matches contents", true, false);
                if (UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), chestStyleMatchesContentsOptions, CSMC_DISABLED)) {
                    if (CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_DISABLED) == CSMC_DISABLED) {
                        CVarSetInteger(CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), 0);
                    }
                }
                UIWidgets::Tooltip(
                    "Chest sizes and textures are changed to help identify the item inside.\n"
                    " - Major items: Large gold chests\n"
                    " - Lesser items: Large brown chests\n"
                    " - Junk items: Small brown chests\n"
                    " - Small keys: Small silver chest\n"
                    " - Boss keys: Vanilla size and texture\n"
                    " - Skulltula Tokens: Small skulltula chest\n"
                    "\n"
                    "NOTE: Textures will not apply if you are using a mod pack with a custom chest model."
                );
                if (CVarGetInteger(CVAR_ENHANCEMENT("ChestSizeAndTextureMatchContents"), CSMC_DISABLED) != CSMC_DISABLED) {
                    UIWidgets::PaddedEnhancementCheckbox("Chests of Agony", CVAR_ENHANCEMENT("ChestSizeDependsStoneOfAgony"), true, false);
                    UIWidgets::Tooltip("Only change the size/texture of chests if you have the Stone of Agony.");
                }
                UIWidgets::PaddedEnhancementCheckbox("Ask to Equip New Items", CVAR_ENHANCEMENT("AskToEquip"), true, false);
                UIWidgets::Tooltip("Adds a prompt to equip newly-obtained swords, shields and tunics");
                UIWidgets::PaddedEnhancementCheckbox("Better Owl", CVAR_ENHANCEMENT("BetterOwl"), true, false);
                UIWidgets::Tooltip("The default response to Kaepora Gaebora is always that you understood what he said");
                UIWidgets::PaddedEnhancementCheckbox("Exit Market at Night", CVAR_ENHANCEMENT("MarketSneak"), true, false);
                UIWidgets::Tooltip("Allows exiting Hyrule Castle Market Town to Hyrule Field at night by speaking to the guard next to the gate.");
                UIWidgets::PaddedEnhancementCheckbox("Shops and Games Always Open", CVAR_ENHANCEMENT("OpenAllHours"), true, false);
                UIWidgets::Tooltip("Shops and minigames are open both day and night. Requires scene reload to take effect.");
                UIWidgets::PaddedEnhancementCheckbox("Link as default file name", CVAR_ENHANCEMENT("LinkDefaultName"), true, false);
                UIWidgets::Tooltip("Allows you to have \"Link\" as a premade file name");
				UIWidgets::PaddedEnhancementCheckbox("Quit Fishing At Door", CVAR_ENHANCEMENT("QuitFishingAtDoor"), true, false);
                UIWidgets::Tooltip("Fisherman asks if you want to quit at the door when you still have the rod");
                UIWidgets::PaddedText("Time Travel with the Song of Time", true, false);
                UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("TimeTravel"), timeTravelOptions, 0);
                UIWidgets::Tooltip("Allows Link to freely change age by playing the Song of Time.\n"
                    "Time Blocks can still be used properly.\n\n"
                    "Requirements:\n"
                    "- Obtained the Ocarina of Time (depends on selection)\n"
                    "- Obtained the Song of Time\n"
                    "- Obtained the Master Sword\n"
                    "- Not within range of Time Block\n"
                    "- Not within range of Ocarina playing spots");
                UIWidgets::PaddedEnhancementCheckbox("Pause Warp", CVAR_ENHANCEMENT("PauseWarp"), true, false);
                UIWidgets::Tooltip("Selection of warp song in pause menu initiates warp. Disables song playback.");
                
                ImGui::EndTable();
                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Items"))
            {
                UIWidgets::PaddedEnhancementCheckbox("Instant Putaway", CVAR_ENHANCEMENT("InstantPutaway"), true, false);
                UIWidgets::Tooltip("Allow Link to put items away without having to wait around");
                UIWidgets::PaddedEnhancementCheckbox("Instant Boomerang Recall", CVAR_ENHANCEMENT("FastBoomerang"), true, false);
                UIWidgets::Tooltip("Instantly return the boomerang to Link by pressing its item button while it's in the air");
                UIWidgets::PaddedEnhancementCheckbox("Prevent Dropped Ocarina Inputs", CVAR_ENHANCEMENT("DpadNoDropOcarinaInput"), true, false);
                UIWidgets::Tooltip("Prevent dropping inputs when playing the ocarina quickly");
                UIWidgets::PaddedText("Bunny Hood Effect", true, false);
                UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("MMBunnyHood"), bunnyHoodOptions, BUNNY_HOOD_VANILLA);
                UIWidgets::Tooltip(
                    "Wearing the Bunny Hood grants a speed increase like in Majora's Mask. The longer jump option is not accounted for in randomizer logic.\n\n"
                    "Also disables NPC's reactions to wearing the Bunny Hood."
                );
                UIWidgets::PaddedEnhancementCheckbox("Bunny Hood Equippable as Adult", CVAR_ENHANCEMENT("AdultBunnyHood"), true, false, (CVarGetInteger(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_VANILLA) == BUNNY_HOOD_VANILLA), "Only available with increased bunny hood speed", UIWidgets::CheckboxGraphics::Cross, false);
                UIWidgets::Tooltip("Allows the bunny hood to be equipped normally from the pause menu as adult.");
                UIWidgets::PaddedEnhancementCheckbox("Mask Select in Inventory", CVAR_ENHANCEMENT("MaskSelect"), true, false);
                UIWidgets::Tooltip("After completing the mask trading sub-quest, press A and any direction on the mask slot to change masks");
                UIWidgets::PaddedEnhancementCheckbox("Nuts explode bombs", CVAR_ENHANCEMENT("NutsExplodeBombs"), true, false);
                UIWidgets::Tooltip("Makes nuts explode bombs, similar to how they interact with bombchus. This does not affect bombflowers.");
                UIWidgets::PaddedEnhancementCheckbox("Equip Multiple Arrows at Once", CVAR_ENHANCEMENT("SeparateArrows"), true, false);
                UIWidgets::Tooltip("Allow the bow and magic arrows to be equipped at the same time on different slots. (Note this will disable the behaviour of the 'Equip Dupe' glitch)");
                UIWidgets::PaddedEnhancementCheckbox("Bow as Child/Slingshot as Adult", CVAR_ENHANCEMENT("BowSlingshotAmmoFix"), true, false);
                UIWidgets::Tooltip("Allows child to use bow with arrows.\nAllows adult to use slingshot with seeds.\n\nRequires glitches or 'Timeless Equipment' cheat to equip.");
                UIWidgets::PaddedEnhancementCheckbox("Better Farore's Wind", CVAR_ENHANCEMENT("BetterFarore"), true, false);
                UIWidgets::Tooltip("Helps FW persist between ages, gives child and adult separate FW points, and can be used in more places.");
                UIWidgets::PaddedEnhancementCheckbox("Remove Explosive Limit", CVAR_ENHANCEMENT("RemoveExplosiveLimit"), true, false);
                UIWidgets::Tooltip("Removes the cap of 3 active explosives being deployed at once.");
                UIWidgets::PaddedEnhancementCheckbox("Static Explosion Radius", CVAR_ENHANCEMENT("StaticExplosionRadius"), true, false);
                UIWidgets::Tooltip("Explosions are now a static size, like in Majora's Mask and OoT3D. Makes bombchu hovering much easier.");
                UIWidgets::PaddedEnhancementCheckbox("Prevent Bombchus Forcing First-Person", CVAR_ENHANCEMENT("DisableFirstPersonChus"), true, false);
                UIWidgets::Tooltip("Prevent bombchus from forcing the camera into first-person mode when released.");
                UIWidgets::PaddedEnhancementCheckbox("Aiming reticle for the bow/slingshot", CVAR_ENHANCEMENT("BowReticle"), true, false);
                UIWidgets::Tooltip("Aiming with a bow or slingshot will display a reticle as with the hookshot when the projectile is ready to fire.");
                if (UIWidgets::PaddedEnhancementCheckbox("Allow strength equipment to be toggled", CVAR_ENHANCEMENT("ToggleStrength"), true, false)) {
                    if (!CVarGetInteger(CVAR_ENHANCEMENT("ToggleStrength"), 0)) {
                        CVarSetInteger(CVAR_ENHANCEMENT("StrengthDisabled"), 0);
                    }
                }
                UIWidgets::Tooltip("Allows strength to be toggled on and off by pressing A on the strength upgrade in the equipment subscreen of the pause menu (This allows performing some glitches that require the player to not have strength).");
                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Difficulty Options"))
            {
                if (ImGui::BeginMenu("Potion Values"))
                {
                    UIWidgets::EnhancementCheckbox("Change Red Potion Effect", CVAR_ENHANCEMENT("RedPotionEffect"));
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Red Potions");
                    bool disabledRedPotion = !CVarGetInteger(CVAR_ENHANCEMENT("RedPotionEffect"), 0);
                    static const char* disabledTooltipRedPotion = "This option is disabled because \"Change Red Potion Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Red Potion Health: %d", "##REDPOTIONHEALTH", CVAR_ENHANCEMENT("RedPotionHealth"), 1, 100, "", 0, true, disabledRedPotion, disabledTooltipRedPotion);
                    UIWidgets::Tooltip("Changes the amount of health restored by Red Potions");
                    UIWidgets::EnhancementCheckbox("Red Potion Percent Restore", CVAR_ENHANCEMENT("RedPercentRestore"), disabledRedPotion, disabledTooltipRedPotion);
                    UIWidgets::Tooltip("Toggles from Red Potions restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Green Potion Effect", CVAR_ENHANCEMENT("GreenPotionEffect"));
                    UIWidgets::Tooltip("Enable the following changes to the amount of mana restored by Green Potions");
                    bool disabledGreenPotion = !CVarGetInteger(CVAR_ENHANCEMENT("GreenPotionEffect"), 0);
                    static const char* disabledTooltipGreenPotion = "This option is disabled because \"Change Green Potion Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Green Potion Mana: %d", "##GREENPOTIONMANA", CVAR_ENHANCEMENT("GreenPotionMana"), 1, 100, "", 0, true, disabledGreenPotion, disabledTooltipGreenPotion);
                    UIWidgets::Tooltip("Changes the amount of mana restored by Green Potions, base max mana is 48, max upgraded mana is 96");
                    UIWidgets::EnhancementCheckbox("Green Potion Percent Restore", CVAR_ENHANCEMENT("GreenPercentRestore"), disabledGreenPotion, disabledTooltipGreenPotion);
                    UIWidgets::Tooltip("Toggles from Green Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Blue Potion Effects", CVAR_ENHANCEMENT("BluePotionEffects"));
                    UIWidgets::Tooltip("Enable the following changes to the amount of health and mana restored by Blue Potions");
                    bool disabledBluePotion = !CVarGetInteger(CVAR_ENHANCEMENT("BluePotionEffects"), 0);
                    static const char* disabledTooltipBluePotion = "This option is disabled because \"Change Blue Potion Effects\" is turned off";
                    UIWidgets::EnhancementSliderInt("Blue Potion Health: %d", "##BLUEPOTIONHEALTH", CVAR_ENHANCEMENT("BluePotionHealth"), 1, 100, "", 0, true, disabledBluePotion, disabledTooltipBluePotion);
                    UIWidgets::Tooltip("Changes the amount of health restored by Blue Potions");
                    UIWidgets::EnhancementCheckbox("Blue Potion Health Percent Restore", CVAR_ENHANCEMENT("BlueHealthPercentRestore"), disabledBluePotion, disabledTooltipBluePotion);
                    UIWidgets::Tooltip("Toggles from Blue Potions restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementSliderInt("Blue Potion Mana: %d", "##BLUEPOTIONMANA", CVAR_ENHANCEMENT("BluePotionMana"), 1, 100, "", 0, true, disabledBluePotion, disabledTooltipBluePotion);
                    UIWidgets::Tooltip("Changes the amount of mana restored by Blue Potions, base max mana is 48, max upgraded mana is 96");
                    UIWidgets::EnhancementCheckbox("Blue Potion Mana Percent Restore", CVAR_ENHANCEMENT("BlueManaPercentRestore"), disabledBluePotion, disabledTooltipBluePotion);
                    UIWidgets::Tooltip("Toggles from Blue Potions restoring a fixed amount of mana to a percent of the player's current max mana");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Milk Effect", CVAR_ENHANCEMENT("MilkEffect"));
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Milk");
                    bool disabledMilk = !CVarGetInteger(CVAR_ENHANCEMENT("MilkEffect"), 0);
                    static const char* disabledTooltipMilk = "This option is disabled because \"Change Milk Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Milk Health: %d", "##MILKHEALTH", CVAR_ENHANCEMENT("MilkHealth"), 1, 100, "", 0, true, disabledMilk, disabledTooltipMilk);
                    UIWidgets::Tooltip("Changes the amount of health restored by Milk");
                    UIWidgets::EnhancementCheckbox("Milk Percent Restore", CVAR_ENHANCEMENT("MilkPercentRestore"), disabledMilk, disabledTooltipMilk);
                    UIWidgets::Tooltip("Toggles from Milk restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Separate Half Milk Effect", CVAR_ENHANCEMENT("SeparateHalfMilkEffect"), disabledMilk, disabledTooltipMilk);
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Half Milk\nIf this is disabled, Half Milk will behave the same as Full Milk.");
                    bool disabledHalfMilk = disabledMilk || !CVarGetInteger(CVAR_ENHANCEMENT("SeparateHalfMilkEffect"), 0);
                    static const char* disabledTooltipHalfMilk = "This option is disabled because \"Separate Half Milk Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Half Milk Health: %d", "##HALFMILKHEALTH", CVAR_ENHANCEMENT("HalfMilkHealth"), 1, 100, "", 0, true, disabledHalfMilk, disabledTooltipHalfMilk);
                    UIWidgets::Tooltip("Changes the amount of health restored by Half Milk");
                    UIWidgets::EnhancementCheckbox("Half Milk Percent Restore", CVAR_ENHANCEMENT("HalfMilkPercentRestore"), disabledHalfMilk, disabledTooltipHalfMilk);
                    UIWidgets::Tooltip("Toggles from Half Milk restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Fairy Effect", CVAR_ENHANCEMENT("FairyEffect"));
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Fairies");
                    bool disabledFairy = !CVarGetInteger(CVAR_ENHANCEMENT("FairyEffect"), 0);
                    static const char* disabledTooltipFairy = "This option is disabled because \"Change Fairy Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Fairy: %d", "##FAIRYHEALTH", CVAR_ENHANCEMENT("FairyHealth"), 1, 100, "", 0, true, disabledFairy, disabledTooltipFairy);
                    UIWidgets::Tooltip("Changes the amount of health restored by Fairies");
                    UIWidgets::EnhancementCheckbox("Fairy Percent Restore", CVAR_ENHANCEMENT("FairyPercentRestore"), disabledFairy, disabledTooltipFairy);
                    UIWidgets::Tooltip("Toggles from Fairies restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::Separator();

                    UIWidgets::EnhancementCheckbox("Change Fairy Revive Effect", CVAR_ENHANCEMENT("FairyReviveEffect"));
                    UIWidgets::Tooltip("Enable the following changes to the amount of health restored by Fairy Revivals");
                    bool disabledFairyRevive = !CVarGetInteger(CVAR_ENHANCEMENT("FairyReviveEffect"), 0);
                    static const char* disabledTooltipFairyRevive = "This option is disabled because \"Change Fairy Revive Effect\" is turned off";
                    UIWidgets::EnhancementSliderInt("Fairy Revival: %d", "##FAIRYREVIVEHEALTH", CVAR_ENHANCEMENT("FairyReviveHealth"), 1, 100, "", 0, true, disabledFairyRevive, disabledTooltipFairyRevive);
                    UIWidgets::Tooltip("Changes the amount of health restored by Fairy Revivals");
                    UIWidgets::EnhancementCheckbox("Fairy Revive Percent Restore", CVAR_ENHANCEMENT("FairyRevivePercentRestore"), disabledFairyRevive, disabledTooltipFairyRevive);
                    UIWidgets::Tooltip("Toggles from Fairy Revivals restoring a fixed amount of health to a percent of the player's current max health");

                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Shooting Gallery")) {
                    UIWidgets::EnhancementCheckbox("Customize Behavior", CVAR_ENHANCEMENT("CustomizeShootingGallery"));
                    UIWidgets::Tooltip("Turn on/off changes to the shooting gallery behavior");
                    bool disabled = !CVarGetInteger(CVAR_ENHANCEMENT("CustomizeShootingGallery"), 0);
                    static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                    UIWidgets::PaddedEnhancementCheckbox("Instant Win", CVAR_ENHANCEMENT("InstantShootingGalleryWin"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Skips the shooting gallery minigame");
                    UIWidgets::PaddedEnhancementCheckbox("No Rupee Randomization", CVAR_ENHANCEMENT("ConstantAdultGallery"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Forces the rupee order to not be randomized as adult, making it the same as chlid");
                    UIWidgets::PaddedEnhancementSliderInt("Child Starting Ammunition: %d", "##cShootingGalleryAmmunition", CVAR_ENHANCEMENT("ShootingGalleryAmmoChild"), 10, 30, "", 15, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The ammunition at the start of the shooting gallery minigame as a child");
                    UIWidgets::PaddedEnhancementSliderInt("Adult Starting Ammunition: %d", "##aShootingGalleryAmmunition", CVAR_ENHANCEMENT("ShootingGalleryAmmoAdult"), 10, 30, "", 15, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The ammunition at the start of the shooting gallery minigame as an adult");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Bombchu Bowling")) {
                    UIWidgets::EnhancementCheckbox("Customize Behavior", CVAR_ENHANCEMENT("CustomizeBombchuBowling"));
                    UIWidgets::Tooltip("Turn on/off changes to the bombchu bowling behavior");
                    bool disabled = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeBombchuBowling"), 0) == 0;
                    static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                    UIWidgets::PaddedEnhancementCheckbox("Remove Small Cucco", CVAR_ENHANCEMENT("BombchuBowlingNoSmallCucco"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Prevents the small cucco from appearing in the bombchu bowling minigame");
                    UIWidgets::PaddedEnhancementCheckbox("Remove Big Cucco", CVAR_ENHANCEMENT("BombchuBowlingNoBigCucco"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Prevents the big cucco from appearing in the bombchu bowling minigame");
                    UIWidgets::PaddedEnhancementSliderInt("Bombchu Count: %d", "##cBombchuBowlingAmmunition", CVAR_ENHANCEMENT("BombchuBowlingAmmo"), 3, 20, "", 10, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The number of bombchus available at the start of the bombchu bowling minigame");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Fishing")) {
                    UIWidgets::EnhancementCheckbox("Customize Behavior", CVAR_ENHANCEMENT("CustomizeFishing"));
                    UIWidgets::Tooltip("Turn on/off changes to the fishing behavior");
                    bool disabled = !CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0);
                    static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                    UIWidgets::PaddedEnhancementCheckbox("Instant Fishing", CVAR_ENHANCEMENT("InstantFishing"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("All fish will be caught instantly");
                    UIWidgets::PaddedEnhancementCheckbox("Guarantee Bite", CVAR_ENHANCEMENT("GuaranteeFishingBite"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("When a line is stable, guarantee bite. Otherwise use default logic");
                    UIWidgets::PaddedEnhancementCheckbox("Fish Never Escape", CVAR_ENHANCEMENT("FishNeverEscape"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Once a hook has been set, fish will never let go while being reeled in.");
                    UIWidgets::PaddedEnhancementSliderInt("Child Minimum Weight: %d", "##cMinimumWeight", CVAR_ENHANCEMENT("MinimumFishWeightChild"), 3, 10, "", 10, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The minimum weight for the unique fishing reward as a child");
                    UIWidgets::PaddedEnhancementSliderInt("Adult Minimum Weight: %d", "##aMinimumWeight", CVAR_ENHANCEMENT("MinimumFishWeightAdult"), 6, 13, "", 13, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("The minimum weight for the unique fishing reward as an adult");
                    ImGui::EndMenu();
                }
                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Lost Woods Ocarina Game")) {
                    UIWidgets::EnhancementCheckbox("Customize Behavior", CVAR_ENHANCEMENT("CustomizeOcarinaGame"));
                    UIWidgets::Tooltip("Turn on/off changes to the lost woods ocarina game behavior");
                    bool disabled = !CVarGetInteger(CVAR_ENHANCEMENT("CustomizeOcarinaGame"), 0);
                    static const char* disabledTooltip = "This option is disabled because \"Customize Behavior\" is turned off";
                    UIWidgets::PaddedEnhancementCheckbox("Instant Win", CVAR_ENHANCEMENT("InstantOcarinaGameWin"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Skips the lost woods ocarina game");
                    UIWidgets::PaddedEnhancementSliderInt("Note Play Speed: %dx", "##OcarinaGameNoteSpeed", CVAR_ENHANCEMENT("OcarinaGame.NoteSpeed"), 1, 5, "", 1, true, true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the speed that the skull kids play notes");
                    UIWidgets::PaddedEnhancementCheckbox("Unlimited Playback Time", CVAR_ENHANCEMENT("OcarinaUnlimitedFailTime"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Removes the timer to play back the song");
                    UIWidgets::PaddedEnhancementSliderInt("Number of Starting Notes: %d", "##OcarinaGameStartingNotes", CVAR_ENHANCEMENT("OcarinaGame.StartingNotes"), 1, 8, "", 3, true, true, false,
                                                          disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the number of notes the skull kids play to start the first round");
                    int roundMin = CVarGetInteger(CVAR_ENHANCEMENT("OcarinaGame.StartingNotes"), 3);
                    UIWidgets::PaddedEnhancementSliderInt("Round One Notes: %d", "##OcarinaGameRoundOne",
                                                          CVAR_ENHANCEMENT("OcarinaGame.RoundOneNotes"), roundMin, 8, "", 5, true, true,
                                                          false,
                                                          disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the number of notes you need to play to end the first round");
                    UIWidgets::PaddedEnhancementSliderInt("Round Two Notes: %d", "##OcarinaGameRoundTwoNotes",
                                                          CVAR_ENHANCEMENT("OcarinaGame.RoundTwoNotes"), roundMin, 8, "", 6, true, true,
                                                          false,
                                                          disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the number of notes you need to play to end the second round");
                    UIWidgets::PaddedEnhancementSliderInt("Round Three Notes: %d", "##OcarinaGameRoundThreeNotes",
                                                          CVAR_ENHANCEMENT("OcarinaGame.RoundThreeNotes"), roundMin, 8, "", 8, true, true,
                                                          false,
                                                          disabled, disabledTooltip);
                    UIWidgets::Tooltip("Adjust the number of notes you need to play to end the third round");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                if (ImGui::BeginMenu("Frogs Ocarina Game")) {
                    UIWidgets::EnhancementCheckbox("Customize Behavior", CVAR_ENHANCEMENT("CustomizeFrogsOcarinaGame"));
                    UIWidgets::Tooltip("Turn on/off changes to the frogs ocarina game behavior");
                    bool disabled = !CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFrogsOcarinaGame"), 0);
                    static const char* disabledTooltip =
                        "This option is disabled because \"Customize Behavior\" is turned off";
                    UIWidgets::PaddedEnhancementCheckbox("Instant Win", CVAR_ENHANCEMENT("InstantFrogsGameWin"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Skips the frogs ocarina game");
                    UIWidgets::PaddedEnhancementCheckbox("Unlimited Playback Time", CVAR_ENHANCEMENT("FrogsUnlimitedFailTime"), true, false, disabled, disabledTooltip);
                    UIWidgets::Tooltip("Removes the timer to play back the song");
                    bool disabledFrog = 0;
                    static const char* disabledFrogTooltip =
                        "This option is disabled because \"Customize Behavior\" is turned off or \"Unlimited Playback Time\" is on";
                    if (!CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFrogsOcarinaGame"), 0) || CVarGetInteger(CVAR_ENHANCEMENT("FrogsUnlimitedFailTime"), 0)) {
                        disabledFrog = 1;
                    } else {
                        disabledFrog = 0;
                    }
                    UIWidgets::PaddedEnhancementSliderInt("Modify note timer: %dx", "##FrogsFailTimer", CVAR_ENHANCEMENT("FrogsModifyFailTime"), 1, 5, "", 1, true, true, false,
                                                          disabledFrog, disabledFrogTooltip);
                    UIWidgets::Tooltip("Adjusts the time allowed for playback before failing");
                    ImGui::EndMenu();
                }

                UIWidgets::Spacer(0);

                UIWidgets::PaddedEnhancementCheckbox("Delete File On Death", CVAR_ENHANCEMENT("DeleteFileOnDeath"), true, false);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                UIWidgets::Tooltip("Dying will delete your file\n\n     " ICON_FA_EXCLAMATION_TRIANGLE " WARNING " ICON_FA_EXCLAMATION_TRIANGLE "\nTHIS IS NOT REVERSABLE\nUSE AT YOUR OWN RISK!");
                ImGui::PopStyleColor();
                if (UIWidgets::PaddedEnhancementCheckbox("Permanent heart loss", CVAR_ENHANCEMENT("PermanentHeartLoss"), true, false)) {
                    UpdatePermanentHeartLossState();
                }
                UIWidgets::Tooltip("When you lose 4 quarters of a heart you will permanently lose that heart container.\n\nDisabling this after the fact will restore your heart containers.");
                ImGui::Text("Damage Multiplier");
                UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("DamageMult"), allPowers, 0);
                UIWidgets::Tooltip(
                    "Modifies all sources of damage not affected by other sliders\n"
                    "2x: Can survive all common attacks from the start of the game\n"
                    "4x: Dies in 1 hit to any substantial attack from the start of the game\n"
                    "8x: Can only survive trivial damage from the start of the game\n"
                    "16x: Can survive all common attacks with max health without double defense\n"
                    "32x: Can survive all common attacks with max health and double defense\n"
                    "64x: Can survive trivial damage with max health without double defense\n"
                    "128x: Can survive trivial damage with max health and double defense\n"
                    "256x: Cannot survive damage"
                );
                UIWidgets::PaddedText("Fall Damage Multiplier", true, false);
                UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("FallDamageMult"), subPowers, 0);
                UIWidgets::Tooltip(
                    "Modifies all fall damage\n"
                    "2x: Can survive all fall damage from the start of the game\n"
                    "4x: Can only survive short fall damage from the start of the game\n"
                    "8x: Cannot survive any fall damage from the start of the game\n"
                    "16x: Can survive all fall damage with max health without double defense\n"
                    "32x: Can survive all fall damage with max health and double defense\n"
                    "64x: Can survive short fall damage with double defense\n"
                    "128x: Cannot survive fall damage"
                );
                UIWidgets::PaddedText("Void Damage Multiplier", true, false);
                UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("VoidDamageMult"), subSubPowers, 0);
                UIWidgets::Tooltip(
                    "Modifies damage taken after falling into a void\n"
                    "2x: Can survive void damage from the start of the game\n"
                    "4x: Cannot survive void damage from the start of the game\n"
                    "8x: Can survive void damage twice with max health without double defense\n"
                    "16x: Can survive void damage with max health without double defense\n"
                    "32x: Can survive void damage with max health and double defense\n"
                    "64x: Cannot survive void damage"
                );
                UIWidgets::PaddedText("Bonk Damage Multiplier", true, false);
                UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("BonkDamageMult"), bonkDamageValues, BONK_DAMAGE_NONE);
                UIWidgets::Tooltip("Modifies damage taken after bonking.");
                UIWidgets::PaddedEnhancementCheckbox("Spawn with full health", CVAR_ENHANCEMENT("FullHealthSpawn"), true, false);
                UIWidgets::Tooltip("Respawn with full health instead of 3 Hearts");
                UIWidgets::PaddedEnhancementCheckbox("No Random Drops", CVAR_ENHANCEMENT("NoRandomDrops"), true, false);
                UIWidgets::Tooltip("Disables random drops, except from the Goron Pot, Dampe, and bosses");
                bool forceEnableBombchuDrops = IS_RANDO &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_ENABLE_BOMBCHU_DROPS) == 1;
                static const char* forceEnableBombchuDropsText =
                    "This setting is forcefully enabled because a savefile\nwith \"Enable Bombchu Drops\" is loaded.";
                UIWidgets::PaddedEnhancementCheckbox("Enable Bombchu Drops", CVAR_ENHANCEMENT("BombchuDrops"), true, false,
                                                        forceEnableBombchuDrops, forceEnableBombchuDropsText, UIWidgets::CheckboxGraphics::Checkmark);
                UIWidgets::Tooltip("Bombchus will sometimes drop in place of bombs");
                UIWidgets::PaddedEnhancementCheckbox("Trees Drop Sticks", CVAR_ENHANCEMENT("TreesDropSticks"), true, false);
                UIWidgets::Tooltip("Bonking into trees will have a chance to drop up to 3 sticks. Must already have obtained sticks.");
                UIWidgets::PaddedEnhancementCheckbox("No Heart Drops", CVAR_ENHANCEMENT("NoHeartDrops"), true, false);
                UIWidgets::Tooltip("Disables heart drops, but not heart placements, like from a Deku Scrub running off\nThis simulates Hero Mode from other games in the series");
                if (UIWidgets::PaddedEnhancementCheckbox("Hyper Bosses", CVAR_ENHANCEMENT("HyperBosses"), true, false)) {
                    UpdateHyperBossesState();
                }
                UIWidgets::Tooltip("All major bosses move and act twice as fast.");
                if (UIWidgets::PaddedEnhancementCheckbox("Hyper Enemies", CVAR_ENHANCEMENT("HyperEnemies"), true, false)) {
                    UpdateHyperEnemiesState();
                }
                UIWidgets::Tooltip("All regular enemies and mini-bosses move and act twice as fast.");
                UIWidgets::PaddedEnhancementCheckbox("Always Win Goron Pot", CVAR_ENHANCEMENT("GoronPot"), true, false);
                UIWidgets::Tooltip("Always get the heart piece/purple rupee from the spinning Goron pot");
                UIWidgets::PaddedEnhancementCheckbox("Always Win Dampe Digging Game", CVAR_ENHANCEMENT("DampeWin"), true, false, SaveManager::Instance->IsRandoFile(),
                                                        "This setting is always enabled in randomizer files", UIWidgets::CheckboxGraphics::Checkmark);
                UIWidgets::Tooltip("Always win the heart piece/purple rupee on the first dig in Dampe's grave digging game, just like in rando\nIn a rando file, this is unconditionally enabled");
                UIWidgets::PaddedEnhancementCheckbox("All Dogs are Richard", CVAR_ENHANCEMENT("AllDogsRichard"), true, false);
                UIWidgets::Tooltip("All dogs can be traded in and will count as Richard.");
                UIWidgets::PaddedEnhancementSliderInt("Cuccos Stay Put Multiplier: %dx", "##CuccoStayDurationMultiplier", CVAR_ENHANCEMENT("CuccoStayDurationMult"), 1, 5, "", 1, true, true, false);
                UIWidgets::Tooltip("Cuccos will stay in place longer after putting them down, by a multiple of the value of the slider.");
                UIWidgets::PaddedEnhancementSliderInt("Leever Spawn Rate: %d seconds", "##LeeverSpawnRate", CVAR_ENHANCEMENT("LeeverSpawnRate"), 0, 10, "", 0, true, true, false);
                UIWidgets::Tooltip("The time between leever groups spawning.");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Reduced Clutter"))
            {
                UIWidgets::EnhancementCheckbox("Mute Low HP Alarm", CVAR_ENHANCEMENT("LowHpAlarm"));
                UIWidgets::Tooltip("Disable the low HP beeping sound");
                UIWidgets::PaddedEnhancementCheckbox("Minimal UI", CVAR_ENHANCEMENT("MinimalUI"), true, false);
                UIWidgets::Tooltip("Hides most of the UI when not needed\nNote: Doesn't activate until after loading a new scene");
                UIWidgets::PaddedEnhancementCheckbox("Disable Navi Call Audio", CVAR_ENHANCEMENT("DisableNaviCallAudio"), true, false);
                UIWidgets::Tooltip("Disables the voice audio when Navi calls you");
                UIWidgets::PaddedEnhancementCheckbox("Disable Hot/Underwater Warning Text", CVAR_ENHANCEMENT("DisableTunicWarningText"), true, false);
                UIWidgets::Tooltip("Disables warning text when you don't have on the Goron/Zora Tunic in Hot/Underwater conditions.");
                UIWidgets::PaddedEnhancementCheckbox("Remember Minimap State Between Areas", CVAR_ENHANCEMENT("RememberMapToggleState"));
                UIWidgets::Tooltip("Preserves the minimap visibility state when going between areas rather than defaulting it to \"on\" when going through loading zones.");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            UIWidgets::EnhancementCheckbox("Visual Stone of Agony", CVAR_ENHANCEMENT("VisualAgony"));
            UIWidgets::Tooltip("Displays an icon and plays a sound when Stone of Agony should be activated, for those without rumble");
            UIWidgets::PaddedEnhancementCheckbox("Assignable Tunics and Boots", CVAR_ENHANCEMENT("AssignableTunicsAndBoots"), true, false);
            UIWidgets::Tooltip("Allows equipping the tunic and boots to c-buttons");
            UIWidgets::PaddedEnhancementCheckbox("Equipment Toggle", CVAR_ENHANCEMENT("EquipmentCanBeRemoved"), true, false);
            UIWidgets::Tooltip("Allows equipment to be removed by toggling it off on\nthe equipment subscreen.");
            if (CVarGetInteger(CVAR_ENHANCEMENT("EquipmentCanBeRemoved"), 0)) {
                UIWidgets::PaddedText("Sword Toggle Options", true, false);
                UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("SwordToggle"), swordToggleModes, SWORD_TOGGLE_NONE);
                UIWidgets::Tooltip(
                    "Introduces Options for unequipping Link's sword\n\n"
                    "None: Only Biggoron's Sword/Giant's Knife can be toggled. Doing so will equip the Master Sword.\n\n"
                    "Child Toggle: This will allow for completely unequipping any sword as child link.\n\n"
                    "Both Ages: Any sword can be unequipped as either age. This may lead to swordless glitches as Adult.\n"
                );
            }

            UIWidgets::PaddedEnhancementCheckbox("Link's Cow in Both Time Periods", CVAR_ENHANCEMENT("CowOfTime"), true, false);
            UIWidgets::Tooltip("Allows the Lon Lon Ranch obstacle course reward to be shared across time periods");
            UIWidgets::PaddedEnhancementCheckbox("Enable visible guard vision", CVAR_ENHANCEMENT("GuardVision"), true, false);
            UIWidgets::PaddedEnhancementCheckbox("Enable passage of time on file select", CVAR_ENHANCEMENT("TimeFlowFileSelect"), true, false);
            UIWidgets::PaddedEnhancementCheckbox("Item counts in messages", CVAR_ENHANCEMENT("InjectItemCounts"), true, false);
            UIWidgets::Tooltip("Injects item counts in pickup messages, like golden skulltula tokens and heart pieces");
            UIWidgets::PaddedEnhancementCheckbox("Pull grave during the day", CVAR_ENHANCEMENT("DayGravePull"), true, false);
            UIWidgets::Tooltip("Allows graves to be pulled when child during the day");
            UIWidgets::PaddedEnhancementCheckbox("Dogs follow you everywhere", CVAR_ENHANCEMENT("DogFollowsEverywhere"), true, false);
            UIWidgets::Tooltip("Allows dogs to follow you anywhere you go, even if you leave the market");
            UIWidgets::PaddedEnhancementCheckbox("Don't require input for Credits sequence", CVAR_ENHANCEMENT("NoInputForCredits"), true, false);
            UIWidgets::Tooltip("Removes the input requirement on textboxes after defeating Ganon, allowing Credits sequence to continue to progress");

            // Blue Fire Arrows
            bool forceEnableBlueFireArrows = IS_RANDO &&
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BLUE_FIRE_ARROWS);
            static const char* forceEnableBlueFireArrowsText =
                "This setting is forcefully enabled because a savefile\nwith \"Blue Fire Arrows\" is loaded.";
            UIWidgets::PaddedEnhancementCheckbox("Blue Fire Arrows", CVAR_ENHANCEMENT("BlueFireArrows"), true, false, 
                forceEnableBlueFireArrows, forceEnableBlueFireArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Allows Ice Arrows to melt red ice.\nMay require a room reload if toggled during gameplay.");

            // Sunlight Arrows
            bool forceEnableSunLightArrows = IS_RANDO &&
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SUNLIGHT_ARROWS);
            static const char* forceEnableSunLightArrowsText =
                "This setting is forcefully enabled because a savefile\nwith \"Sunlight Arrows\" is loaded.";
            UIWidgets::PaddedEnhancementCheckbox("Sunlight Arrows", CVAR_ENHANCEMENT("SunlightArrows"), true, false, 
                forceEnableSunLightArrows, forceEnableSunLightArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Allows Light Arrows to activate sun switches.\nMay require a room reload if toggled during gameplay.");

            UIWidgets::PaddedEnhancementCheckbox("Disable Crit wiggle", CVAR_ENHANCEMENT("DisableCritWiggle"), true, false);
            UIWidgets::Tooltip("Disable random camera wiggle at low health");
            UIWidgets::PaddedEnhancementCheckbox("Enemy Health Bars", CVAR_ENHANCEMENT("EnemyHealthBar"), true, false);
            UIWidgets::Tooltip("Renders a health bar for enemies when Z-Targeted");

            UIWidgets::PaddedEnhancementCheckbox("Targetable Hookshot Reticle", CVAR_ENHANCEMENT("HookshotableReticle"), true, false);
            UIWidgets::Tooltip("Use a different color when aiming at hookshotable collision");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Graphics"))
        {
            if (ImGui::BeginMenu("Mods")) {
                UIWidgets::PaddedEnhancementCheckbox("Use Alternate Assets", CVAR_ENHANCEMENT("AltAssets"), false, false);
                UIWidgets::Tooltip("Toggle between standard assets and alternate assets. Usually mods will indicate if this setting has to be used or not.");
                UIWidgets::PaddedEnhancementCheckbox("Disable Bomb Billboarding", CVAR_ENHANCEMENT("DisableBombBillboarding"), true, false);
                UIWidgets::Tooltip("Disables bombs always rotating to face the camera. To be used in conjunction with mods that want to replace bombs with 3D objects.");
                UIWidgets::PaddedEnhancementCheckbox("Disable Grotto Fixed Rotation", CVAR_ENHANCEMENT("DisableGrottoRotation"), true, false);
                UIWidgets::Tooltip("Disables grottos rotating with the camera. To be used in conjunction with mods that want to replace grottos with 3D objects.");
                UIWidgets::PaddedEnhancementCheckbox("Invisible Bunny Hood", CVAR_ENHANCEMENT("HideBunnyHood"), true, false);
                UIWidgets::Tooltip("Turns Bunny Hood invisible while still maintaining its effects.");
                UIWidgets::PaddedEnhancementCheckbox("Disable HUD Heart animations", CVAR_ENHANCEMENT("NoHUDHeartAnimation"), true, false);
                UIWidgets::Tooltip("Disables the beating animation of the hearts on the HUD.");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Animated Link in Pause Menu")) {
                ImGui::Text("Rotation");
                UIWidgets::EnhancementRadioButton("Disabled", CVAR_ENHANCEMENT("PauseLiveLinkRotation"), 0);
                UIWidgets::EnhancementRadioButton("Rotate Link with D-pad", CVAR_ENHANCEMENT("PauseLiveLinkRotation"), 1);
                UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the D-pad\nUse D-pad Up or D-pad Down to reset Link's rotation");
                UIWidgets::EnhancementRadioButton("Rotate Link with C-buttons", CVAR_ENHANCEMENT("PauseLiveLinkRotation"), 2);
                UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the C-buttons\nUse C-Up or C-Down to reset Link's rotation");
                UIWidgets::EnhancementRadioButton("Rotate Link with Right Stick", CVAR_ENHANCEMENT("PauseLiveLinkRotation"), 3);
                UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the Right Stick\nYou can zoom in by pointing up and reset Link's rotation by pointing down");
                if (CVarGetInteger(CVAR_ENHANCEMENT("PauseLiveLinkRotation"), 0) != 0) {
                    UIWidgets::EnhancementSliderInt("Rotation Speed: %d", "##MinRotationSpeed", CVAR_ENHANCEMENT("PauseLiveLinkRotationSpeed"), 1, 20, "", 1);
                }
                UIWidgets::PaddedSeparator();
                ImGui::Text("Static loop");
                UIWidgets::EnhancementRadioButton("Disabled", CVAR_ENHANCEMENT("PauseLiveLink"), 0);
                UIWidgets::EnhancementRadioButton("Idle (standing)", CVAR_ENHANCEMENT("PauseLiveLink"), 1);
                UIWidgets::EnhancementRadioButton("Idle (look around)", CVAR_ENHANCEMENT("PauseLiveLink"), 2);
                UIWidgets::EnhancementRadioButton("Idle (belt)", CVAR_ENHANCEMENT("PauseLiveLink"), 3);
                UIWidgets::EnhancementRadioButton("Idle (shield)", CVAR_ENHANCEMENT("PauseLiveLink"), 4);
                UIWidgets::EnhancementRadioButton("Idle (test sword)", CVAR_ENHANCEMENT("PauseLiveLink"), 5);
                UIWidgets::EnhancementRadioButton("Idle (yawn)", CVAR_ENHANCEMENT("PauseLiveLink"), 6);
                UIWidgets::EnhancementRadioButton("Battle Stance", CVAR_ENHANCEMENT("PauseLiveLink"), 7);
                UIWidgets::EnhancementRadioButton("Walking (no shield)", CVAR_ENHANCEMENT("PauseLiveLink"), 8);
                UIWidgets::EnhancementRadioButton("Walking (holding shield)", CVAR_ENHANCEMENT("PauseLiveLink"), 9);
                UIWidgets::EnhancementRadioButton("Running (no shield)", CVAR_ENHANCEMENT("PauseLiveLink"), 10);
                UIWidgets::EnhancementRadioButton("Running (holding shield)", CVAR_ENHANCEMENT("PauseLiveLink"), 11);
                UIWidgets::EnhancementRadioButton("Hand on hip", CVAR_ENHANCEMENT("PauseLiveLink"), 12);
                UIWidgets::EnhancementRadioButton("Spin attack charge", CVAR_ENHANCEMENT("PauseLiveLink"), 13);
                UIWidgets::EnhancementRadioButton("Look at hand", CVAR_ENHANCEMENT("PauseLiveLink"), 14);
                UIWidgets::PaddedSeparator();
                ImGui::Text("Randomize");
                UIWidgets::EnhancementRadioButton("Random", CVAR_ENHANCEMENT("PauseLiveLink"), 15);
                UIWidgets::Tooltip("Randomize the animation played each time you open the menu");
                UIWidgets::EnhancementRadioButton("Random cycle", CVAR_ENHANCEMENT("PauseLiveLink"), 16);
                UIWidgets::Tooltip("Randomize the animation played on the menu after a certain time");
                UIWidgets::EnhancementRadioButton("Random cycle (Idle)", CVAR_ENHANCEMENT("PauseLiveLink"), 17);
                UIWidgets::Tooltip("Randomize the animation played on the menu after a certain time (Idle animations only)");
                if (CVarGetInteger(CVAR_ENHANCEMENT("PauseLiveLink"), 0) >= 16) {
                    UIWidgets::EnhancementSliderInt("Frame to wait: %d", "##MinFrameCount", CVAR_ENHANCEMENT("MinFrameCount"), 1, 1000, "", 0);
                }

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            UIWidgets::PaddedEnhancementCheckbox("Disable LOD", CVAR_ENHANCEMENT("DisableLOD"), true, false);
            UIWidgets::Tooltip(
                "Turns off the Level of Detail setting, making models use their higher-poly variants at any distance");
            if (UIWidgets::EnhancementSliderInt("Increase Actor Draw Distance: %dx", "##IncreaseActorDrawDistance",
                                                CVAR_ENHANCEMENT("DisableDrawDistance"), 1, 5, "", 1, true, false)) {
                if (CVarGetInteger(CVAR_ENHANCEMENT("DisableDrawDistance"), 1) <= 1) {
                    CVarSetInteger(CVAR_ENHANCEMENT("DisableKokiriDrawDistance"), 0);
                }
            }
            UIWidgets::Tooltip("Increases the range in which actors/objects are drawn");
            if (CVarGetInteger(CVAR_ENHANCEMENT("DisableDrawDistance"), 1) > 1) {
                UIWidgets::PaddedEnhancementCheckbox("Kokiri Draw Distance",
                                                     CVAR_ENHANCEMENT("DisableKokiriDrawDistance"), true, false);
                UIWidgets::Tooltip("The Kokiri are mystical beings that fade into view when approached\nEnabling this "
                                   "will remove their draw distance");
            }
            UIWidgets::PaddedEnhancementCheckbox("Widescreen Actor Culling", CVAR_ENHANCEMENT("WidescreenActorCulling"),
                                                 true, false);
            UIWidgets::Tooltip("Adjusts the horizontal culling plane to account for widescreen resolutions");
            UIWidgets::PaddedEnhancementCheckbox(
                "Cull Glitch Useful Actors", CVAR_ENHANCEMENT("ExtendedCullingExcludeGlitchActors"), true, false,
                !CVarGetInteger(CVAR_ENHANCEMENT("WidescreenActorCulling"), 0) &&
                    CVarGetInteger(CVAR_ENHANCEMENT("DisableDrawDistance"), 1) <= 1,
                "Requires Actor Draw Distance to be increased or Widescreen Actor Culling enabled");
            UIWidgets::Tooltip(
                "Exclude actors that are useful for glitches from the extended culling ranges.\n"
                "Some actors may still draw in the extended ranges, but will not \"update\" so that certain "
                "glitches that leverage the original culling requirements will still work.\n"
                "\n"
                "The following actors are excluded:\n"
                "- White clothed Gerudos\n"
                "- King Zora\n"
                "- Gossip Stones\n"
                "- Boulders\n"
                "- Blue Warps\n"
                "- Darunia\n"
                "- Gold Skulltulas");
            if (UIWidgets::PaddedEnhancementCheckbox("Show Age-Dependent Equipment", CVAR_ENHANCEMENT("EquimentAlwaysVisible"), true,
                                                     false)) {
                UpdatePatchHand();
            }
            UIWidgets::Tooltip("Makes all equipment visible, regardless of Age.");
            if (CVarGetInteger(CVAR_ENHANCEMENT("EquimentAlwaysVisible"), 0) == 1) {
				UIWidgets::PaddedEnhancementCheckbox("Scale Adult Equipment as Child", CVAR_ENHANCEMENT("ScaleAdultEquimentAsChild"), true, false);
				UIWidgets::Tooltip("Scales all of the Adult Equipment, as well and moving some a bit, to fit on Child Link Better. May not work properly with some mods.");
			}
            UIWidgets::PaddedEnhancementCheckbox("N64 Mode", CVAR_LOW_RES_MODE, true, false);
            UIWidgets::Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
            UIWidgets::PaddedEnhancementCheckbox("Glitch line-up tick", CVAR_ENHANCEMENT("DrawLineupTick"), true, false);
            UIWidgets::Tooltip("Displays a tick in the top center of the screen to help with glitch line-ups in SoH, as traditional UI based line-ups do not work outside of 4:3");
            UIWidgets::PaddedEnhancementCheckbox("Enable 3D Dropped items/projectiles", CVAR_ENHANCEMENT("NewDrops"), true, false);
            UIWidgets::Tooltip("Change most 2D items and projectiles on the overworld to their 3D versions");
            UIWidgets::PaddedEnhancementCheckbox("Disable Black Bar Letterboxes", CVAR_ENHANCEMENT("DisableBlackBars"), true, false);
            UIWidgets::Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual glitches that were covered up by the black bars\nPlease disable this setting before reporting a bug");
            UIWidgets::PaddedEnhancementCheckbox("Dynamic Wallet Icon", CVAR_ENHANCEMENT("DynamicWalletIcon"), true, false);
            UIWidgets::Tooltip("Changes the rupee in the wallet icon to match the wallet size you currently have");
            UIWidgets::PaddedEnhancementCheckbox("Always show dungeon entrances", CVAR_ENHANCEMENT("AlwaysShowDungeonMinimapIcon"), true, false);
            UIWidgets::Tooltip("Always shows dungeon entrance icons on the minimap");
            UIWidgets::PaddedEnhancementCheckbox("Show Gauntlets in First Person", CVAR_ENHANCEMENT("FirstPersonGauntlets"), true, false);
            UIWidgets::Tooltip("Renders Gauntlets when using the Bow and Hookshot like in OOT3D");
            if (UIWidgets::PaddedEnhancementCheckbox("Color Temple of Time's Medallions", CVAR_ENHANCEMENT("ToTMedallionsColors"), true, false)) {
                PatchToTMedallions();
            }
            UIWidgets::Tooltip("When medallions are collected, the medallion imprints around the Master Sword pedestal in the Temple of Time will become colored");
            UIWidgets::PaddedEnhancementCheckbox("Show locked door chains on both sides of locked doors", CVAR_ENHANCEMENT("ShowDoorLocksOnBothSides"), true, false);
            UIWidgets::PaddedText("Fix Vanishing Paths", true, false);
            if (UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("SceneSpecificDirtPathFix"), zFightingOptions, ZFIGHT_FIX_DISABLED) && gPlayState != NULL) {
                UpdateDirtPathFixState(gPlayState->sceneNum);
            }
            UIWidgets::Tooltip("Disabled: Paths vanish more the higher the resolution (Z-fighting is based on resolution)\n"
                                "Consistent: Certain paths vanish the same way in all resolutions\n"
                                "No Vanish: Paths do not vanish, Link seems to sink in to some paths\n"
                                "This might affect other decal effects\n");
            UIWidgets::PaddedEnhancementSliderInt("Text Spacing: %d", "##TEXTSPACING", CVAR_ENHANCEMENT("TextSpacing"), 4, 6, "", 6, true, true, true);
            UIWidgets::Tooltip("Space between text characters (useful for HD font textures)");
            UIWidgets::PaddedEnhancementCheckbox("More info in file select", CVAR_ENHANCEMENT("FileSelectMoreInfo"), true, false);
            UIWidgets::Tooltip("Shows what items you have collected in the file select screen, like in N64 randomizer");
            UIWidgets::PaddedEnhancementCheckbox("Better ammo rendering in pause menu", CVAR_ENHANCEMENT("BetterAmmoRendering"), true, false);
            UIWidgets::Tooltip("Ammo counts in the pause menu will work correctly regardless of the position of items in the inventory");
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Fixes"))
        {
            UIWidgets::EnhancementCheckbox("Fix L&R Pause menu", CVAR_ENHANCEMENT("FixMenuLR"));
            UIWidgets::Tooltip("Makes the L and R buttons in the pause menu the same color");
            UIWidgets::PaddedEnhancementCheckbox("Fix L&Z Page switch in Pause menu", CVAR_ENHANCEMENT("NGCKaleidoSwitcher"), true, false);
            UIWidgets::Tooltip("Makes L and R switch pages like on the GameCube\nZ opens the Debug Menu instead");
            UIWidgets::PaddedEnhancementCheckbox("Fix Dungeon entrances", CVAR_ENHANCEMENT("FixDungeonMinimapIcon"), true, false);
            UIWidgets::Tooltip("Removes the dungeon entrance icon on the top-left corner of the screen when no dungeon is present on the current map");
            UIWidgets::PaddedEnhancementCheckbox("Fix Two Handed idle animations", CVAR_ENHANCEMENT("TwoHandedIdle"), true, false);
            UIWidgets::Tooltip("Re-enables the two-handed idle animation, a seemingly finished animation that was disabled on accident in the original game");
            UIWidgets::PaddedEnhancementCheckbox("Fix the Gravedigging Tour Glitch", CVAR_ENHANCEMENT("GravediggingTourFix"), true, false);
            UIWidgets::Tooltip("Fixes a bug where the Gravedigging Tour Heart Piece disappears if the area reloads");
            UIWidgets::PaddedEnhancementCheckbox("Fix Deku Nut upgrade", CVAR_ENHANCEMENT("DekuNutUpgradeFix"), true, false);
            UIWidgets::Tooltip("Prevents the Forest Stage Deku Nut upgrade from becoming unobtainable after receiving the Poacher's Saw");
            UIWidgets::PaddedEnhancementCheckbox("Fix Navi text HUD position", CVAR_ENHANCEMENT("NaviTextFix"), true, false);
            UIWidgets::Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button");
            UIWidgets::PaddedEnhancementCheckbox("Fix Anubis fireballs", CVAR_ENHANCEMENT("AnubisFix"), true, false);
            UIWidgets::Tooltip("Make Anubis fireballs do fire damage when reflected back at them with the Mirror Shield");
            if (UIWidgets::PaddedEnhancementCheckbox("Fix Megaton Hammer crouch stab", CVAR_ENHANCEMENT("CrouchStabHammerFix"), true, false)) {
                if (!CVarGetInteger(CVAR_ENHANCEMENT("CrouchStabHammerFix"), 0)) {
                    CVarClear(CVAR_ENHANCEMENT("CrouchStabFix"));
                }
            }
            UIWidgets::Tooltip("Make the Megaton Hammer's crouch stab able to destroy rocks without first swinging it normally");
            if (CVarGetInteger(CVAR_ENHANCEMENT("CrouchStabHammerFix"), 0)) {
                UIWidgets::PaddedEnhancementCheckbox("Remove power crouch stab", CVAR_ENHANCEMENT("CrouchStabFix"), true, false);
                UIWidgets::Tooltip("Make crouch stabbing always do the same damage as a regular slash");
            }
            UIWidgets::PaddedEnhancementCheckbox("Fix credits timing", CVAR_ENHANCEMENT("CreditsFix"), true, false);
            UIWidgets::Tooltip("Extend certain credits scenes so the music lines up properly with the visuals");
            UIWidgets::PaddedEnhancementCheckbox("Fix Gerudo Warrior's clothing colors", CVAR_ENHANCEMENT("GerudoWarriorClothingFix"), true, false);
            UIWidgets::Tooltip("Prevent the Gerudo Warrior's clothes changing color when changing Link's tunic or using bombs in front of her");
            UIWidgets::PaddedEnhancementCheckbox("Fix Camera Drift", CVAR_ENHANCEMENT("FixCameraDrift"), true, false);
            UIWidgets::Tooltip("Fixes camera slightly drifting to the left when standing still due to a math error");
            UIWidgets::PaddedEnhancementCheckbox("Fix Camera Swing", CVAR_ENHANCEMENT("FixCameraSwing"), true, false);
            UIWidgets::Tooltip("Fixes camera getting stuck on collision when standing still, also fixes slight shift back in camera when stop moving");
            UIWidgets::PaddedEnhancementCheckbox("Fix Hanging Ledge Swing Rate", CVAR_ENHANCEMENT("FixHangingLedgeSwingRate"), true, false);
            UIWidgets::Tooltip("Fixes camera swing rate when player falls off a ledge and camera swings around");
            UIWidgets::PaddedEnhancementCheckbox("Fix Missing Jingle after 5 Silver Rupees", CVAR_ENHANCEMENT("SilverRupeeJingleExtend"), true, false);
            UIWidgets::Tooltip(
                "Adds 5 higher pitches for the Silver Rupee Jingle for the rooms with more than 5 Silver Rupees. "
                "Currently only relevant in Master Quest.");
            if (UIWidgets::PaddedEnhancementCheckbox("Fix out of bounds textures", CVAR_ENHANCEMENT("FixTexturesOOB"), true, false)) {
                ApplyAuthenticGfxPatches();
            }
            UIWidgets::Tooltip("Fixes authentic out of bounds texture reads, instead loading textures with the correct size");
            UIWidgets::PaddedEnhancementCheckbox("Fix Poacher's Saw Softlock", CVAR_ENHANCEMENT("FixSawSoftlock"), true, false, CVarGetInteger(CVAR_ENHANCEMENT("SkipText"), 0),
                "This is disabled because it is forced on when Skip Text is enabled.", UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Prevents the Poacher's Saw softlock from mashing through the text, or with Skip Text enabled.");
            UIWidgets::PaddedEnhancementCheckbox("Fix enemies not spawning near water", CVAR_ENHANCEMENT("EnemySpawnsOverWaterboxes"), true, false);
            UIWidgets::Tooltip("Causes respawning enemies, like stalchildren, to appear on land near bodies of water. "
                                "Fixes an incorrect calculation that acted like water underneath ground was above it.");
            UIWidgets::PaddedEnhancementCheckbox("Fix Bush Item Drops", CVAR_ENHANCEMENT("BushDropFix"), true, false);
            UIWidgets::Tooltip("Fixes the bushes to drop items correctly rather than spawning undefined items.");
            UIWidgets::PaddedEnhancementCheckbox("Fix falling from vine edges", CVAR_ENHANCEMENT("FixVineFall"), true, false); 
            UIWidgets::Tooltip("Prevents immediately falling off climbable surfaces if climbing on the edges."); 
            UIWidgets::PaddedEnhancementCheckbox("Fix Link's eyes open while sleeping", CVAR_ENHANCEMENT("FixEyesOpenWhileSleeping"), true, false);
            UIWidgets::Tooltip("Fixes Link's eyes being open in the opening cutscene when he is supposed to be sleeping.");
            UIWidgets::PaddedEnhancementCheckbox("Fix Darunia dancing too fast", CVAR_ENHANCEMENT("FixDaruniaDanceSpeed"),
                                                 true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip("Fixes Darunia's dancing speed so he dances to the beat of Saria's Song, like in vanilla.");
            UIWidgets::PaddedEnhancementCheckbox("Fix raised Floor Switches", CVAR_ENHANCEMENT("FixFloorSwitches"), true, false);
            UIWidgets::Tooltip("Fixes the two raised floor switches, the one in Forest Temple Basement and the one at the top of Fire Temple. \n"
                "This will lower them, making activating them easier");
            UIWidgets::PaddedEnhancementCheckbox("Fix Zora hint dialogue", CVAR_ENHANCEMENT("FixZoraHintDialogue"), true, false);
            UIWidgets::Tooltip("Fixes one Zora's dialogue giving a hint about bringing Ruto's Letter to King Zora to properly occur before moving King Zora rather than after");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Restoration"))
        {
            UIWidgets::EnhancementCheckbox("Authentic Logo Screen", CVAR_ENHANCEMENT("AuthenticLogo"));
            UIWidgets::Tooltip("Hide the game version and build details and display the authentic model and texture on the boot logo start screen");
            UIWidgets::PaddedEnhancementCheckbox("Red Ganon blood", CVAR_ENHANCEMENT("RedGanonBlood"), true, false);
            UIWidgets::Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for green blood");
            UIWidgets::PaddedEnhancementCheckbox("Fish while hovering", CVAR_ENHANCEMENT("HoverFishing"), true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows casting the Fishing Rod while using the Hover Boots");
            UIWidgets::PaddedEnhancementCheckbox("N64 Weird Frames", CVAR_ENHANCEMENT("N64WeirdFrames"), true, false);
            UIWidgets::Tooltip("Restores N64 Weird Frames allowing weirdshots to behave the same as N64");
            UIWidgets::PaddedEnhancementCheckbox("Bombchus out of bounds", CVAR_ENHANCEMENT("BombchusOOB"), true, false);
            UIWidgets::Tooltip("Allows bombchus to explode out of bounds\nSimilar to GameCube and Wii VC");
            UIWidgets::PaddedEnhancementCheckbox("Quick Putaway", CVAR_ENHANCEMENT("QuickPutaway"), true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows putting away an item without an animation and performing Putaway Ocarina Items");
            UIWidgets::PaddedEnhancementCheckbox("Restore old Gold Skulltula cutscene", CVAR_ENHANCEMENT("GSCutscene"), true, false);
            UIWidgets::Tooltip("Restore pre-release behavior where defeating a Gold Skulltula will play a cutscene showing it die.");
            UIWidgets::PaddedEnhancementCheckbox("Quick Bongo Kill", CVAR_ENHANCEMENT("QuickBongoKill"), true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows bypassing Bongo Bongo's intro cutscene to quickly kill him");
            UIWidgets::PaddedEnhancementCheckbox("Original RBA Values", CVAR_ENHANCEMENT("RestoreRBAValues"), true, false);
            UIWidgets::Tooltip("Restores the original outcomes when performing Reverse Bottle Adventure.");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Extra Modes")) {
            UIWidgets::PaddedText("Mirrored World", true, false);
            if (UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("MirroredWorldMode"), mirroredWorldModes, MIRRORED_WORLD_OFF) && gPlayState != NULL) {
                UpdateMirrorModeState(gPlayState->sceneNum);
            }
            UIWidgets::Tooltip(
                "Mirrors the world horizontally\n\n"
                "- Always: Always mirror the world\n"
                "- Random: Randomly decide to mirror the world on each scene change\n"
                "- Random (Seeded): Scenes are mirrored based on the current randomizer seed/file\n"
                "- Dungeons: Mirror the world in Dungeons\n"
                "- Dungeons (Vanilla): Mirror the world in vanilla Dungeons\n"
                "- Dungeons (MQ): Mirror the world in MQ Dungeons\n"
                "- Dungeons Random: Randomly decide to mirror the world in Dungeons\n"
                "- Dungeons Random (Seeded): Dungeons are mirrored based on the current randomizer seed/file\n"
            );

            UIWidgets::PaddedText("Enemy Randomizer", true, false);
            UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("RandomizedEnemies"), enemyRandomizerModes, ENEMY_RANDOMIZER_OFF);
            UIWidgets::Tooltip(
                "Replaces fixed enemies throughout the game with a random enemy. Bosses, mini-bosses and a few specific regular enemies are excluded.\n"
                "Enemies that need more than Deku Nuts + either Deku Sticks or a sword to kill are excluded from spawning in \"clear enemy\" rooms.\n\n"
                "- Random: Enemies are randomized every time you load a room\n"
                "- Random (Seeded): Enemies are randomized based on the current randomizer seed/file\n"
            );

            UIWidgets::PaddedEnhancementCheckbox("Randomized Enemy Sizes", CVAR_ENHANCEMENT("RandomizedEnemySizes"), true, false);
            UIWidgets::Tooltip("Enemies and Bosses spawn with random sizes.");

            if (CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemySizes"), 0)) {
                UIWidgets::EnhancementCheckbox("Scale Health with Size", CVAR_ENHANCEMENT("EnemySizeScalesHealth"));
                UIWidgets::Tooltip("Scales normal enemies health with their randomized size. *This will NOT affect bosses*");
            }

            UIWidgets::PaddedEnhancementCheckbox("Ivan the Fairy (Coop Mode)", CVAR_ENHANCEMENT("IvanCoopModeEnabled"), true, false);
            UIWidgets::Tooltip("Enables Ivan the Fairy upon the next map change. Player 2 can control Ivan and "
                                "press the C-Buttons to use items and mess with Player 1!");

            UIWidgets::PaddedEnhancementCheckbox("Rupee Dash Mode", CVAR_ENHANCEMENT("RupeeDash"), true, false);
            UIWidgets::Tooltip("Rupees reduced over time, Link suffers damage when the count hits 0.");

            if (CVarGetInteger(CVAR_ENHANCEMENT("RupeeDash"), 0)) {
                UIWidgets::PaddedEnhancementSliderInt(
                    "Rupee Dash Interval: %d", "##DashInterval", CVAR_ENHANCEMENT("RupeeDashInterval"), 1, 10, "", 5, true, true, false,
                    !CVarGetInteger(CVAR_ENHANCEMENT("RupeeDash"), 0),
                    "This option is disabled because \"Rupee Dash Mode\" is turned off");
                UIWidgets::Tooltip("Interval between Rupee reduction in Rupee Dash Mode");
            }

            UIWidgets::PaddedEnhancementCheckbox("Shadow Tag Mode", CVAR_ENHANCEMENT("ShadowTag"), true, false);
            UIWidgets::Tooltip("A wallmaster follows Link everywhere, don't get caught!");

            UIWidgets::PaddedEnhancementCheckbox("Additional Traps", CVAR_ENHANCEMENT("ExtraTraps.Enabled"), true, false);
            UIWidgets::Tooltip("Enables additional Trap variants.");

            if (CVarGetInteger(CVAR_ENHANCEMENT("ExtraTraps.Enabled"), 0)) {
                UIWidgets::PaddedSeparator();
                if (ImGui::BeginMenu("Trap Options")) {
                    ImGui::Text("Tier 1 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Freeze Traps", CVAR_ENHANCEMENT("ExtraTraps.Ice"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Burn Traps", CVAR_ENHANCEMENT("ExtraTraps.Burn"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Shock Traps", CVAR_ENHANCEMENT("ExtraTraps.Shock"), true, false);

                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Tier 2 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Knockback Traps", CVAR_ENHANCEMENT("ExtraTraps.Knockback"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Speed Traps", CVAR_ENHANCEMENT("ExtraTraps.Speed"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Bomb Traps", CVAR_ENHANCEMENT("ExtraTraps.Bomb"), true, false);

                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Tier 3 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Void Traps", CVAR_ENHANCEMENT("ExtraTraps.Void"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Ammo Traps", CVAR_ENHANCEMENT("ExtraTraps.Ammo"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Death Traps", CVAR_ENHANCEMENT("ExtraTraps.Kill"), true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Teleport Traps", CVAR_ENHANCEMENT("ExtraTraps.Teleport"), true, false);

                    ImGui::EndMenu();
                }
            }

            UIWidgets::Spacer(0);
            if (UIWidgets::PaddedEnhancementCheckbox("Hurt Container Mode", CVAR_ENHANCEMENT("HurtContainer"), true, false)) {
                UpdateHurtContainerModeState(CVarGetInteger(CVAR_ENHANCEMENT("HurtContainer"), 0));
            }
            UIWidgets::Tooltip("Changes Heart Piece and Heart Container functionality.\n\n"
                "- Each Heart Container or full Heart Piece reduces Links hearts by 1.\n"
                "- Can be enabled retroactively after a File has already started.");

            ImGui::EndMenu();
        }

        UIWidgets::PaddedSeparator(false, true);

        // Autosave enum value of 1 is the default in presets and the old checkbox "on" state for backwards compatibility
        UIWidgets::PaddedText("Autosave", false, true);
        UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("Autosave"), autosaveLabels, AUTOSAVE_OFF);
        UIWidgets::Tooltip("Automatically save the game when changing locations and/or obtaining items\n"
            "Major items exclude rupees and health/magic/ammo refills (but include bombchus unless bombchu drops are enabled)");

        UIWidgets::PaddedSeparator(true, true, 2.0f, 2.0f);

        ImGui::EndDisabled();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));

        if (mCosmeticsEditorWindow) {
            if (ImGui::Button(GetWindowButtonText("Cosmetics Editor", CVarGetInteger(CVAR_WINDOW("CosmeticsEditor"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mCosmeticsEditorWindow->ToggleVisibility();
            }
        }

        if (mAudioEditorWindow) {
            if (ImGui::Button(GetWindowButtonText("Audio Editor", CVarGetInteger(CVAR_WINDOW("AudioEditor"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mAudioEditorWindow->ToggleVisibility();
            }
        }

        if (mGameplayStatsWindow) {
            if (ImGui::Button(GetWindowButtonText("Gameplay Stats", CVarGetInteger(CVAR_WINDOW("GameplayStats"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mGameplayStatsWindow->ToggleVisibility();
            }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        #ifdef __SWITCH__
        UIWidgets::Spacer(0);
        ImGui::Text("Switch performance mode");
        if (UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("SwitchPerfMode"), SWITCH_CPU_PROFILES, (int)Ship::SwitchProfiles::STOCK)) {
            SPDLOG_INFO("Profile:: %s", SWITCH_CPU_PROFILES[CVarGetInteger(CVAR_ENHANCEMENT("SwitchPerfMode"), (int)Ship::SwitchProfiles::STOCK)]);
            Ship::Switch::ApplyOverclock();
        }
        #endif

        ImGui::EndMenu();
    }
}

void DrawCheatsMenu() {
    if (ImGui::BeginMenu("Cheats"))
    {
        ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
        ImGui::BeginTable("##cheatsMenu", 2, ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextColumn();
        UIWidgets::Spacer(2.0f);
        ImGui::Text("Inventory:");
        UIWidgets::PaddedSeparator();

        UIWidgets::PaddedEnhancementCheckbox("Super Tunic", CVAR_CHEAT("SuperTunic"), true, false);
        UIWidgets::Tooltip("Makes every tunic have the effects of every other tunic");
        UIWidgets::PaddedEnhancementCheckbox("Easy ISG", CVAR_CHEAT("EasyISG"), true, false);
        UIWidgets::Tooltip("Passive Infinite Sword Glitch\nIt makes your sword's swing effect and hitbox stay active indefinitely");
        UIWidgets::PaddedEnhancementCheckbox("Easy QPA", CVAR_CHEAT("EasyQPA"), true, false);
        UIWidgets::Tooltip("Gives you the glitched damage value of the quick put away glitch.");
        UIWidgets::PaddedEnhancementCheckbox("Timeless Equipment", CVAR_CHEAT("TimelessEquipment"), true, false);
        UIWidgets::Tooltip("Allows any item to be equipped, regardless of age\nAlso allows Child to use Adult strength upgrades");
        UIWidgets::PaddedEnhancementCheckbox("Unrestricted Items", CVAR_CHEAT("NoRestrictItems"), true, false);
        UIWidgets::Tooltip("Allows you to use any item at any location");
        UIWidgets::PaddedEnhancementCheckbox("Fireproof Deku Shield", CVAR_CHEAT("FireproofDekuShield"), true, false);
        UIWidgets::Tooltip("Prevents the Deku Shield from burning on contact with fire");
        UIWidgets::PaddedEnhancementCheckbox("Shield with Two-Handed Weapons", CVAR_CHEAT("ShieldTwoHanded"), true, false);
        UIWidgets::Tooltip("This allows you to put up your shield with any two-handed weapon in hand except for Deku Sticks");
        UIWidgets::Spacer(2.0f);
        ImGui::Text("Deku Sticks:");
        UIWidgets::EnhancementCombobox(CVAR_CHEAT("DekuStick"), DekuStickCheat, DEKU_STICK_NORMAL);
        UIWidgets::Spacer(2.0f);
        UIWidgets::EnhancementSliderFloat("Bomb Timer Multiplier: %.2fx", "##gBombTimerMultiplier", CVAR_CHEAT("BombTimerMultiplier"), 0.1f, 5.0f, "", 1.0f, false);
        UIWidgets::PaddedEnhancementCheckbox("Hookshot Everything", CVAR_CHEAT("HookshotEverything"), true, false);
        UIWidgets::Tooltip("Makes every surface in the game hookshot-able");
        UIWidgets::Spacer(0);
        UIWidgets::EnhancementSliderFloat("Hookshot Reach Multiplier: %.2fx", "##gCheatHookshotReachMultiplier", CVAR_CHEAT("HookshotReachMultiplier"), 1.0f, 5.0f, "", 1.0f, false);
        UIWidgets::Spacer(2.0f);
        if (ImGui::Button("Change Age")) {
            CVarSetInteger(CVAR_GENERAL("SwitchAge"), 1);
        }
        UIWidgets::Tooltip("Switches Link's age and reloads the area.");  
        UIWidgets::Spacer(2.0f);
        if (ImGui::Button("Clear Cutscene Pointer")) {
            GameInteractor::RawAction::ClearCutscenePointer();
        }
        UIWidgets::Tooltip("Clears the cutscene pointer to a value safe for wrong warps.");   

        ImGui::TableNextColumn();
        UIWidgets::Spacer(2.0f);

        if (ImGui::BeginMenu("Infinite...")) {
            UIWidgets::EnhancementCheckbox("Money", CVAR_CHEAT("InfiniteMoney"));
            UIWidgets::PaddedEnhancementCheckbox("Health", CVAR_CHEAT("InfiniteHealth"), true, false);
            UIWidgets::PaddedEnhancementCheckbox("Ammo", CVAR_CHEAT("InfiniteAmmo"), true, false);
            UIWidgets::PaddedEnhancementCheckbox("Magic", CVAR_CHEAT("InfiniteMagic"), true, false);
            UIWidgets::PaddedEnhancementCheckbox("Nayru's Love", CVAR_CHEAT("InfiniteNayru"), true, false);
            UIWidgets::PaddedEnhancementCheckbox("Epona Boost", CVAR_CHEAT("InfiniteEponaBoost"), true, false);

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Save States")) {
            ImGui::TextColored({ 0.85f, 0.85f, 0.0f, 1.0f }, "          " ICON_FA_EXCLAMATION_TRIANGLE);
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, " WARNING!!!! ");
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.85f, 0.0f, 1.0f }, ICON_FA_EXCLAMATION_TRIANGLE);
            UIWidgets::PaddedText("These are NOT like emulator states.", true, false);
            UIWidgets::PaddedText("They do not save your game progress, and", true, false);
            UIWidgets::PaddedText("they WILL break across transitions and", true, false);
            UIWidgets::PaddedText("load zones (like doors). Support for", true, false);
            UIWidgets::PaddedText("related issues will not be provided.", true, false);
            if (UIWidgets::PaddedEnhancementCheckbox("I promise I have read the warning", CVAR_CHEAT("SaveStatePromise"), true,
                                                     false)) {
                CVarSetInteger(CVAR_CHEAT("SaveStatesEnabled"), 0);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }
            if (CVarGetInteger(CVAR_CHEAT("SaveStatePromise"), 0) == 1) {
                UIWidgets::PaddedEnhancementCheckbox("I understand, enable save states", CVAR_CHEAT("SaveStatesEnabled"), true,
                                                     false);
                UIWidgets::Tooltip("F5 to save, F6 to change slots, F7 to load");
            }

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(2.0f);
        ImGui::Text("Behavior:");
        UIWidgets::PaddedSeparator();

        UIWidgets::PaddedEnhancementCheckbox("No Clip", CVAR_CHEAT("NoClip"), true, false);
        UIWidgets::Tooltip("Allows you to walk through walls");
        UIWidgets::PaddedEnhancementCheckbox("Climb Everything", CVAR_CHEAT("ClimbEverything"), true, false);
        UIWidgets::Tooltip("Makes every surface in the game climbable");
        UIWidgets::PaddedEnhancementCheckbox("Moon Jump on L", CVAR_CHEAT("MoonJumpOnL"), true, false);
        UIWidgets::Tooltip("Holding L makes you float into the air");
        UIWidgets::PaddedEnhancementCheckbox("Easy Frame Advancing", CVAR_CHEAT("EasyPauseBuffer"), true, false);
        UIWidgets::Tooltip("Continue holding START button when unpausing to only advance a single frame and then re-pause");
        const bool bEasyFrameAdvanceEnabled = CVarGetInteger(CVAR_CHEAT("EasyPauseBuffer"), 0);
        UIWidgets::PaddedEnhancementCheckbox("Easy Input Buffering", CVAR_CHEAT("EasyInputBuffer"), true, false, bEasyFrameAdvanceEnabled, "Forced enabled when Easy Frame Advancing is enabled", UIWidgets::CheckboxGraphics::Checkmark);
        UIWidgets::Tooltip("Inputs that are held down while the Subscreen is closing will be pressed when the game is resumed");
        UIWidgets::PaddedEnhancementCheckbox("Drops Don't Despawn", CVAR_CHEAT("DropsDontDie"), true, false);
        UIWidgets::Tooltip("Drops from enemies, grass, etc. don't disappear after a set amount of time");
        UIWidgets::PaddedEnhancementCheckbox("Fish Don't despawn", CVAR_CHEAT("NoFishDespawn"), true, false);
        UIWidgets::Tooltip("Prevents fish from automatically despawning after a while when dropped");
        UIWidgets::PaddedEnhancementCheckbox("Bugs Don't despawn", CVAR_CHEAT("NoBugsDespawn"), true, false);
        UIWidgets::Tooltip("Prevents bugs from automatically despawning after a while when dropped");
        UIWidgets::PaddedEnhancementCheckbox("Freeze Time", CVAR_CHEAT("FreezeTime"), true, false);
        UIWidgets::Tooltip("Freezes the time of day");
        UIWidgets::PaddedEnhancementCheckbox("Time Sync", CVAR_CHEAT("TimeSync"), true, false);
        UIWidgets::Tooltip("This syncs the ingame time with the real world time");
        UIWidgets::PaddedEnhancementCheckbox("No ReDead/Gibdo Freeze", CVAR_CHEAT("NoRedeadFreeze"), true, false);
        UIWidgets::Tooltip("Prevents ReDeads and Gibdos from being able to freeze you with their scream");
        UIWidgets::PaddedEnhancementCheckbox("Keese/Guay don't target you", CVAR_CHEAT("NoKeeseGuayTarget"), true, false);
        UIWidgets::Tooltip("Keese and Guay no longer target you and simply ignore you as if you were wearing the skull mask");
        {
            static int32_t betaQuestEnabled = CVarGetInteger(CVAR_CHEAT("EnableBetaQuest"), 0);
            static int32_t lastBetaQuestEnabled = betaQuestEnabled;
            static int32_t betaQuestWorld = CVarGetInteger(CVAR_CHEAT("BetaQuestWorld"), 0xFFEF);
            static int32_t lastBetaQuestWorld = betaQuestWorld;

            if (!isBetaQuestEnabled) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }

            UIWidgets::PaddedEnhancementCheckbox("Enable Beta Quest", CVAR_CHEAT("EnableBetaQuest"), true, false);
            UIWidgets::Tooltip("Turns on OoT Beta Quest. *WARNING* This will reset your game.");
            betaQuestEnabled = CVarGetInteger(CVAR_CHEAT("EnableBetaQuest"), 0);
            if (betaQuestEnabled) {
                if (betaQuestEnabled != lastBetaQuestEnabled) {
                    betaQuestWorld = 0;
                }

                ImGui::Text("Beta Quest World: %d", betaQuestWorld);

                if (ImGui::Button(" - ##BetaQuest")) {
                    betaQuestWorld--;
                }
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);

                ImGui::SliderInt("##BetaQuest", &betaQuestWorld, 0, 8, "", ImGuiSliderFlags_AlwaysClamp);
                UIWidgets::Tooltip("Set the Beta Quest world to explore. *WARNING* Changing this will reset your game.\nCtrl+Click to type in a value.");

                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 7.0f);
                if (ImGui::Button(" + ##BetaQuest")) {
                    betaQuestWorld++;
                }

                if (betaQuestWorld > 8) {
                    betaQuestWorld = 8;
                }
                else if (betaQuestWorld < 0) {
                    betaQuestWorld = 0;
                }
            }
            else {
                lastBetaQuestWorld = betaQuestWorld = 0xFFEF;
                CVarClear(CVAR_CHEAT("BetaQuestWorld"));
            }
            if (betaQuestEnabled != lastBetaQuestEnabled || betaQuestWorld != lastBetaQuestWorld)
            {
                // Reset the game if the beta quest state or world changed because beta quest happens on redirecting the title screen cutscene.
                lastBetaQuestEnabled = betaQuestEnabled;
                lastBetaQuestWorld = betaQuestWorld;
                CVarSetInteger(CVAR_CHEAT("EnableBetaQuest"), betaQuestEnabled);
                CVarSetInteger(CVAR_CHEAT("BetaQuestWorld"), betaQuestWorld);

                std::reinterpret_pointer_cast<Ship::ConsoleWindow>(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }

            if (!isBetaQuestEnabled) {
                UIWidgets::ReEnableComponent("");
            }
        }

        ImGui::EndTable();
        ImGui::EndDisabled();
        ImGui::EndMenu();
    }
}

extern std::shared_ptr<Ship::GuiWindow> mStatsWindow;
extern std::shared_ptr<Ship::GuiWindow> mConsoleWindow;
extern std::shared_ptr<SaveEditorWindow> mSaveEditorWindow;
extern std::shared_ptr<ColViewerWindow> mColViewerWindow;
extern std::shared_ptr<ActorViewerWindow> mActorViewerWindow;
extern std::shared_ptr<DLViewerWindow> mDLViewerWindow;
extern std::shared_ptr<ValueViewerWindow> mValueViewerWindow;
extern std::shared_ptr<MessageViewer> mMessageViewerWindow;

void DrawDeveloperToolsMenu() {
    if (ImGui::BeginMenu("Developer Tools")) {
        ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));

        UIWidgets::EnhancementCheckbox("OoT Debug Mode", CVAR_DEVELOPER_TOOLS("DebugEnabled"));
        UIWidgets::Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right, and open the debug menu with L on the pause screen");
        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("DebugEnabled"), 0)) {
            UIWidgets::EnhancementCheckbox("OoT Registry Editor", CVAR_DEVELOPER_TOOLS("RegEditEnabled"));
            UIWidgets::Tooltip("Enables the registry editor");
            ImGui::Text("Debug Save File Mode:");
            UIWidgets::EnhancementCombobox(CVAR_DEVELOPER_TOOLS("DebugSaveFileMode"), DebugSaveFileModes, 1);
            UIWidgets::Tooltip(
                "Changes the behaviour of debug file select creation (creating a save file on slot 1 with debug mode on)\n"
                "- Off: The debug save file will be a normal savefile\n"
                "- Vanilla: The debug save file will be the debug save file from the original game\n"
                "- Maxed: The debug save file will be a save file with all of the items & upgrades"
            );
        }
        UIWidgets::PaddedEnhancementCheckbox("OoT Skulltula Debug", CVAR_DEVELOPER_TOOLS("SkulltulaDebugEnabled"), true, false);
        UIWidgets::Tooltip("Enables Skulltula Debug, when moving the cursor in the menu above various map icons (boss key, compass, map screen locations, etc) will set the GS bits in that area.\nUSE WITH CAUTION AS IT DOES NOT UPDATE THE GS COUNT.");
        UIWidgets::PaddedEnhancementCheckbox("Fast File Select", CVAR_DEVELOPER_TOOLS("SkipLogoTitle"), true, false);
        UIWidgets::Tooltip("Load the game to the selected menu or file\n\"Zelda Map Select\" require debug mode else you will fallback to File choose menu\nUsing a file number that don't have save will create a save file only if you toggle on \"Create a new save if none ?\" else it will bring you to the File choose menu");
        if (CVarGetInteger(CVAR_DEVELOPER_TOOLS("SkipLogoTitle"), 0)) {
            ImGui::Text("Loading:");
            UIWidgets::EnhancementCombobox(CVAR_DEVELOPER_TOOLS("SaveFileID"), FastFileSelect, 0);
        };
        UIWidgets::PaddedEnhancementCheckbox("Better Debug Warp Screen", CVAR_DEVELOPER_TOOLS("BetterDebugWarpScreen"), true, false);
        UIWidgets::Tooltip("Optimized debug warp screen, with the added ability to chose entrances and time of day");
        UIWidgets::PaddedEnhancementCheckbox("Debug Warp Screen Translation", CVAR_DEVELOPER_TOOLS("DebugWarpScreenTranslation"), true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
        UIWidgets::Tooltip("Translate the Debug Warp Screen based on the game language");
        UIWidgets::PaddedEnhancementCheckbox("Resource logging", CVAR_DEVELOPER_TOOLS("ResourceLogging"), true, false);
        UIWidgets::Tooltip("Logs some resources as XML when they're loaded in binary format");
        if (gPlayState != NULL) {
            UIWidgets::PaddedSeparator();
            ImGui::Checkbox("Frame Advance##frameAdvance", (bool*)&gPlayState->frameAdvCtx.enabled);
            if (gPlayState->frameAdvCtx.enabled) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                if (ImGui::Button("Advance 1", ImVec2(ImGui::GetContentRegionAvail().x / 2.0f, 0.0f))) {
                    CVarSetInteger(CVAR_GENERAL("FrameAdvance"), 1);
                }
                ImGui::SameLine();
                ImGui::Button("Advance (Hold)");
                if (ImGui::IsItemActive()) {
                    CVarSetInteger(CVAR_GENERAL("FrameAdvance"), 1);
                }
                ImGui::PopStyleVar(3);
                ImGui::PopStyleColor(1);
            }
        }
        UIWidgets::PaddedSeparator();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
        if (mStatsWindow) {
            if (ImGui::Button(GetWindowButtonText("Stats", CVarGetInteger(CVAR_STATS_WINDOW_OPEN, 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mStatsWindow->ToggleVisibility();
            }
            UIWidgets::Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
        }
        UIWidgets::Spacer(0);
        if (mConsoleWindow) {
            if (ImGui::Button(GetWindowButtonText("Console", CVarGetInteger(CVAR_CONSOLE_WINDOW_OPEN, 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mConsoleWindow->ToggleVisibility();
            }
            UIWidgets::Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
        }
        UIWidgets::Spacer(0);
        if (mSaveEditorWindow) {
            if (ImGui::Button(GetWindowButtonText("Save Editor", CVarGetInteger(CVAR_WINDOW("SaveEditor"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mSaveEditorWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mColViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Collision Viewer", CVarGetInteger(CVAR_WINDOW("CollisionViewer"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mColViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mActorViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Actor Viewer", CVarGetInteger(CVAR_WINDOW("ActorViewer"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mActorViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mDLViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Display List Viewer", CVarGetInteger(CVAR_WINDOW("DLViewer"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mDLViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mValueViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Value Viewer", CVarGetInteger(CVAR_WINDOW("ValueViewer"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mValueViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mMessageViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Message Viewer", CVarGetInteger(CVAR_WINDOW("MessageViewer"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mMessageViewerWindow->ToggleVisibility();
            }
        }

        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        ImGui::EndDisabled();

        ImGui::EndMenu();
    }
}

bool isStringEmpty(std::string str) {
    // Remove spaces at the beginning of the string
    std::string::size_type start = str.find_first_not_of(' ');
    // Remove spaces at the end of the string
    std::string::size_type end = str.find_last_not_of(' ');

    // Check if the string is empty after stripping spaces
    if (start == std::string::npos || end == std::string::npos)
        return true; // The string is empty
    else
        return false; // The string is not empty
}

#ifdef ENABLE_REMOTE_CONTROL
void DrawRemoteControlMenu() {
    if (ImGui::BeginMenu("Network")) {
        static std::string ip = CVarGetString(CVAR_REMOTE("IP"), "127.0.0.1");
        static uint16_t port = CVarGetInteger(CVAR_REMOTE("Port"), 43384);
        bool isFormValid = !isStringEmpty(CVarGetString(CVAR_REMOTE("IP"), "127.0.0.1")) && port > 1024 && port < 65535;

        const char* remoteOptions[2] = { "Sail", "Crowd Control"};

        ImGui::BeginDisabled(GameInteractor::Instance->isRemoteInteractorEnabled);
        ImGui::Text("Remote Interaction Scheme");
        if (UIWidgets::EnhancementCombobox(CVAR_REMOTE("Scheme"), remoteOptions, GI_SCHEME_SAIL)) {
            auto scheme = CVarGetInteger(CVAR_REMOTE("Scheme"), GI_SCHEME_SAIL);
            switch (scheme) {
                case GI_SCHEME_SAIL:
                case GI_SCHEME_CROWD_CONTROL:
                    CVarSetString(CVAR_REMOTE("IP"), "127.0.0.1");
                    CVarSetInteger(CVAR_REMOTE("Port"), 43384);
                    ip = "127.0.0.1";
                    port = 43384;
                    break;
            }
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }
        switch (CVarGetInteger(CVAR_REMOTE("Scheme"), GI_SCHEME_SAIL)) {
            case GI_SCHEME_SAIL:
                UIWidgets::InsertHelpHoverText(
                    "Sail is a networking protocol designed to facilitate remote "
                    "control of the Ship of Harkinian client. It is intended to "
                    "be utilized alongside a Sail server, for which we provide a "
                    "few straightforward implementations on our GitHub. The current "
                    "implementations available allow integration with Twitch chat "
                    "and SAMMI Bot, feel free to contribute your own!\n"
                    "\n"
                    "Click the question mark to copy the link to the Sail Github "
                    "page to your clipboard."
                );
                if (ImGui::IsItemClicked()) {
                    ImGui::SetClipboardText("https://github.com/HarbourMasters/sail");
                }
                break;
            case GI_SCHEME_CROWD_CONTROL:
                UIWidgets::InsertHelpHoverText(
                    "Crowd Control is a platform that allows viewers to interact "
                    "with a streamer's game in real time.\n"
                    "\n"
                    "Click the question mark to copy the link to the Crowd Control "
                    "website to your clipboard."
                );
                if (ImGui::IsItemClicked()) {
                    ImGui::SetClipboardText("https://crowdcontrol.live");
                }
                break;
        }

        ImGui::Text("Remote IP & Port");
        if (ImGui::InputText("##gRemote.IP", (char*)ip.c_str(), ip.capacity() + 1)) {
            CVarSetString(CVAR_REMOTE("IP"), ip.c_str());
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 5);
        if (ImGui::InputScalar("##gRemote.Port", ImGuiDataType_U16, &port)) {
            CVarSetInteger(CVAR_REMOTE("Port"), port);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }

        ImGui::PopItemWidth();
        ImGui::EndDisabled();

        ImGui::Spacing();

        ImGui::BeginDisabled(!isFormValid);
        const char* buttonLabel = GameInteractor::Instance->isRemoteInteractorEnabled ? "Disable" : "Enable";
        if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
            if (GameInteractor::Instance->isRemoteInteractorEnabled) {
                CVarClear(CVAR_REMOTE("Enabled"));
                CVarClear(CVAR_REMOTE("CrowdControl"));
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                switch (CVarGetInteger(CVAR_REMOTE("Scheme"), GI_SCHEME_SAIL)) {
                    case GI_SCHEME_SAIL:
                        GameInteractorSail::Instance->Disable();
                        break;
                    case GI_SCHEME_CROWD_CONTROL:
                        CrowdControl::Instance->Disable();
                        break;
                }
            } else {
                CVarSetInteger(CVAR_REMOTE("Enabled"), 1);
                Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                switch (CVarGetInteger(CVAR_REMOTE("Scheme"), GI_SCHEME_SAIL)) {
                    case GI_SCHEME_SAIL:
                        GameInteractorSail::Instance->Enable();
                        break;
                    case GI_SCHEME_CROWD_CONTROL:
                        CrowdControl::Instance->Enable();
                        break;
                }
            }
        }
        ImGui::EndDisabled();

        if (GameInteractor::Instance->isRemoteInteractorEnabled) {
            ImGui::Spacing();
            if (GameInteractor::Instance->isRemoteInteractorConnected) {
                ImGui::Text("Connected");
            } else {
                ImGui::Text("Connecting...");
            }
        }

        ImGui::Dummy(ImVec2(0.0f, 0.0f));
        ImGui::EndMenu();
    }
}
#endif

extern std::shared_ptr<RandomizerSettingsWindow> mRandomizerSettingsWindow;
extern std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
extern std::shared_ptr<ItemTrackerSettingsWindow> mItemTrackerSettingsWindow;
extern std::shared_ptr<EntranceTrackerWindow> mEntranceTrackerWindow;
extern std::shared_ptr<CheckTracker::CheckTrackerWindow> mCheckTrackerWindow;
extern std::shared_ptr<CheckTracker::CheckTrackerSettingsWindow> mCheckTrackerSettingsWindow;

void DrawRandomizerMenu() {
    if (ImGui::BeginMenu("Randomizer")) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
    #ifdef __WIIU__
        static ImVec2 buttonSize(200.0f * 2.0f, 0.0f);
    #else
        static ImVec2 buttonSize(200.0f, 0.0f);
    #endif
        if (mRandomizerSettingsWindow) {
            if (ImGui::Button(GetWindowButtonText("Randomizer Settings", CVarGetInteger(CVAR_WINDOW("RandomizerSettings"), 0)).c_str(), buttonSize)) {
                mRandomizerSettingsWindow->ToggleVisibility();
            }
        }

        UIWidgets::Spacer(0);
        if (mItemTrackerWindow) {
            if (ImGui::Button(GetWindowButtonText("Item Tracker", CVarGetInteger(CVAR_WINDOW("ItemTracker"), 0)).c_str(), buttonSize)) {
                mItemTrackerWindow->ToggleVisibility();
            }
        }

        UIWidgets::Spacer(0);
        if (mItemTrackerSettingsWindow) {
            if (ImGui::Button(GetWindowButtonText("Item Tracker Settings", CVarGetInteger(CVAR_WINDOW("ItemTrackerSettings"), 0)).c_str(), buttonSize)) {
                mItemTrackerSettingsWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mEntranceTrackerWindow) {
            if (ImGui::Button(GetWindowButtonText("Entrance Tracker", CVarGetInteger(CVAR_WINDOW("EntranceTracker"), 0)).c_str(), buttonSize)) {
                mEntranceTrackerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mCheckTrackerWindow) {
            if (ImGui::Button(GetWindowButtonText("Check Tracker", CVarGetInteger(CVAR_WINDOW("CheckTracker"), 0)).c_str(), buttonSize)) {
                mCheckTrackerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mCheckTrackerSettingsWindow) {
            if (ImGui::Button(GetWindowButtonText("Check Tracker Settings", CVarGetInteger(CVAR_WINDOW("CheckTrackerSettings"), 0)).c_str(), buttonSize)) {
                mCheckTrackerSettingsWindow->ToggleVisibility();
            }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        UIWidgets::PaddedSeparator();

        if (ImGui::BeginMenu("Rando Enhancements"))
        {
            UIWidgets::EnhancementCheckbox("Rando-Relevant Navi Hints", CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"), false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "Replace Navi's overworld quest hints with rando-related gameplay hints."
            );
            UIWidgets::PaddedEnhancementCheckbox("Random Rupee Names", CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"), true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "When obtaining rupees, randomize what the rupee is called in the textbox."
            );

            // Only disable the key colors checkbox when none of the keysanity settings are set to "Any Dungeon", "Overworld" or "Anywhere"
            bool disableKeyColors = true;

            if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANY_DUNGEON ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OVERWORLD ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANYWHERE ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_KEYS) != RO_GERUDO_KEYS_VANILLA ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANY_DUNGEON ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_OVERWORLD ||
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) == RO_DUNGEON_ITEM_LOC_ANYWHERE ||
                (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_VANILLA &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_OWN_DUNGEON &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_STARTWITH) || 
                !IS_RANDO) {
                disableKeyColors = false;
            }

            static const char* disableKeyColorsText = 
                "This setting is disabled because a savefile is loaded without any key\n"
                "shuffle settings set to \"Any Dungeon\", \"Overworld\" or \"Anywhere\"";

            UIWidgets::PaddedEnhancementCheckbox("Key Colors Match Dungeon", CVAR_RANDOMIZER_ENHANCEMENT("MatchKeyColors"), true, false,
                                                  disableKeyColors, disableKeyColorsText, UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "Matches the color of small keys and boss keys to the dungeon they belong to. "
                "This helps identify keys from afar and adds a little bit of flair.\n\nThis only "
                "applies to seeds with keys and boss keys shuffled to Any Dungeon, Overworld, or Anywhere.");
            UIWidgets::PaddedEnhancementCheckbox("Quest Item Fanfares", CVAR_RANDOMIZER_ENHANCEMENT("QuestItemFanfares"), true, false);
            UIWidgets::Tooltip(
                "Play unique fanfares when obtaining quest items "
                "(medallions/stones/songs). Note that these fanfares are longer than usual."
            );
            UIWidgets::PaddedEnhancementCheckbox("Mysterious Shuffled Items", CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), true, false);
            UIWidgets::Tooltip(
                "Displays a \"Mystery Item\" model in place of any freestanding/GS/shop items that were shuffled, "
                "and replaces item names for them and scrubs and merchants, regardless of hint settings, "
                "so you never know what you're getting.");
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void SohMenuBar::InitElement() {
    UpdateWindowBackendObjects();
}

void SohMenuBar::DrawElement() {
    if (ImGui::BeginMenuBar()) {
        DrawMenuBarIcon();

        static ImVec2 sWindowPadding(8.0f, 8.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sWindowPadding);

        DrawShipMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawSettingsMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawEnhancementsMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawCheatsMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawDeveloperToolsMenu();

        ImGui::SetCursorPosY(0.0f);

        #ifdef ENABLE_REMOTE_CONTROL
        DrawRemoteControlMenu();

        ImGui::SetCursorPosY(0.0f);
        #endif

        DrawRandomizerMenu();

        ImGui::PopStyleVar(1);
        ImGui::EndMenuBar();
    }
}
} // namespace SohGui
