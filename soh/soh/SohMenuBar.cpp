#include "SohMenuBar.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "ImGui/imgui.h"
#include "regex"
#include "public/bridge/consolevariablebridge.h"
#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "include/z64audio.h"
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
#include "Enhancements/controls/GameControlEditor.h"
#include "Enhancements/cosmetics/CosmeticsEditor.h"
#include "Enhancements/debugger/actorViewer.h"
#include "Enhancements/debugger/colViewer.h"
#include "Enhancements/debugger/debugSaveEditor.h"
#include "Enhancements/debugger/dlViewer.h"
#include "Enhancements/debugger/valueViewer.h"
#include "Enhancements/gameplaystatswindow.h"
#include "Enhancements/randomizer/randomizer_check_tracker.h"
#include "Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "Enhancements/randomizer/randomizer_item_tracker.h"
#include "Enhancements/randomizer/randomizer_settings_window.h"
#include "Enhancements/resolution-editor/ResolutionEditor.h"

extern bool ToggleAltAssetsAtEndOfFrame;
extern bool isBetaQuestEnabled;

extern "C" PlayState* gPlayState;

enum SeqPlayers {
    /* 0 */ SEQ_BGM_MAIN,
    /* 1 */ SEQ_FANFARE,
    /* 2 */ SEQ_SFX,
    /* 3 */ SEQ_BGM_SUB,
    /* 4 */ SEQ_MAX
};

std::string GetWindowButtonText(const char* text, bool menuOpen) {
    char buttonText[100] = "";
    if (menuOpen) {
        strcat(buttonText, ICON_FA_CHEVRON_RIGHT " ");
    }
    strcat(buttonText, text);
    if (!menuOpen) { strcat(buttonText, "  "); }
    return buttonText;
}

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
    static const char* uiMenuColors[9] = { "White", "Gray", "Indigo", "Red", "Dark Red", "Light Green", "Green", "Dark Green", "Yellow" };
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
    static ImVec4 colorChoice;

extern "C" SaveContext gSaveContext;

namespace SohGui {

void DrawMenuBarIcon() {
    static bool gameIconLoaded = false;
    if (!gameIconLoaded) {
        LUS::Context::GetInstance()->GetWindow()->GetGui()->LoadTexture("Game_Icon", "textures/icons/gIcon.png");
        gameIconLoaded = true;
    }

    if (LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Game_Icon")) {
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
        ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Game_Icon"), iconSize);
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
            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetMenuBar()->ToggleVisibility();
        }
        UIWidgets::Spacer(0);
#if !defined(__SWITCH__) && !defined(__WIIU__)
        if (ImGui::MenuItem("Toggle Fullscreen", "F11")) {
            LUS::Context::GetInstance()->GetWindow()->ToggleFullscreen();
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
            std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
        }
#if !defined(__SWITCH__) && !defined(__WIIU__)
        UIWidgets::Spacer(0);
        if (ImGui::MenuItem("Open App Files Folder")) {
            std::string filesPath = LUS::Context::GetInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        }
        UIWidgets::Spacer(0);

        if (ImGui::MenuItem("Quit")) {
            LUS::Context::GetInstance()->GetWindow()->Close();
        }
#endif
        ImGui::EndMenu();
    }
}

extern std::shared_ptr<LUS::GuiWindow> mInputEditorWindow;
extern std::shared_ptr<GameControlEditor::GameControlEditorWindow> mGameControlEditorWindow;
extern std::shared_ptr<AdvancedResolutionSettings::AdvancedResolutionSettingsWindow> mAdvancedResolutionSettingsWindow;

void DrawSettingsMenu() {
    if (ImGui::BeginMenu("Settings"))
    {
        if (ImGui::BeginMenu("Audio")) {
            UIWidgets::PaddedEnhancementSliderFloat("Master Volume: %.1f %%", "##Master_Vol", "gGameMasterVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true);
            if (UIWidgets::PaddedEnhancementSliderFloat("Main Music Volume: %.1f %%", "##Main_Music_Vol", "gMainMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_BGM_MAIN, CVarGetFloat("gMainMusicVolume", 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Sub Music Volume: %.1f %%", "##Sub_Music_Vol", "gSubMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_BGM_SUB, CVarGetFloat("gSubMusicVolume", 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Sound Effects Volume: %.1f %%", "##Sound_Effect_Vol", "gSFXMusicVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_SFX, CVarGetFloat("gSFXMusicVolume", 1.0f));
            }
            if (UIWidgets::PaddedEnhancementSliderFloat("Fanfare Volume: %.1f %%", "##Fanfare_Vol", "gFanfareVolume", 0.0f, 1.0f, "", 1.0f, true, true, false, true)) {
                Audio_SetGameVolume(SEQ_FANFARE, CVarGetFloat("gFanfareVolume", 1.0f));
            }

            static std::unordered_map<LUS::AudioBackend, const char*> audioBackendNames = {
                { LUS::AudioBackend::WASAPI, "Windows Audio Session API" },
                { LUS::AudioBackend::PULSE, "PulseAudio" },
                { LUS::AudioBackend::SDL, "SDL" },
            };

            ImGui::Text("Audio API (Needs reload)");
            auto currentAudioBackend = LUS::Context::GetInstance()->GetAudio()->GetAudioBackend();

            if (LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::BeginCombo("##AApi", audioBackendNames[currentAudioBackend])) {
                for (uint8_t i = 0; i < LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size(); i++) {
                    auto backend = LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->data()[i];
                    if (ImGui::Selectable(audioBackendNames[backend], backend == currentAudioBackend)) {
                        LUS::Context::GetInstance()->GetAudio()->SetAudioBackend(backend);
                    }
                }
                ImGui::EndCombo();
            }
            if (LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1) {
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
                if (ImGui::Button(GetWindowButtonText("Controller Mapping", CVarGetInteger("gControllerConfigurationEnabled", 0)).c_str(), ImVec2 (-1.0f, 0.0f))) {
                    mInputEditorWindow->ToggleVisibility();
                }
            }
            if (mGameControlEditorWindow) {
                if (ImGui::Button(GetWindowButtonText("Additional Controller Options", CVarGetInteger("gGameControlEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                    mGameControlEditorWindow->ToggleVisibility();
                }
            }
            UIWidgets::PaddedSeparator();
            ImGui::PopStyleColor(1);
            ImGui::PopStyleVar(3);
        #ifndef __SWITCH__
            UIWidgets::EnhancementCheckbox("Menubar Controller Navigation", "gControlNav");
            UIWidgets::Tooltip("Allows controller navigation of the SOH menu bar (Settings, Enhancements,...)\nCAUTION: This will disable game inputs while the menubar is visible.\n\nD-pad to move between items, A to select, and X to grab focus on the menu bar");
        #endif
            UIWidgets::PaddedEnhancementCheckbox("Show Inputs", "gInputEnabled", true, false);
            UIWidgets::Tooltip("Shows currently pressed inputs on the bottom right of the screen");
            UIWidgets::PaddedEnhancementSliderFloat("Input Scale: %.2f", "##Input", "gInputScale", 1.0f, 3.0f, "", 1.0f, false, true, true, false);
            UIWidgets::Tooltip("Sets the on screen size of the displayed inputs from the Show Inputs setting");
            UIWidgets::PaddedEnhancementSliderInt("Simulated Input Lag: %d frames", "##SimulatedInputLag", "gSimulatedInputLag", 0, 6, "", 0, true, true, false);
            UIWidgets::Tooltip("Buffers your inputs to be executed a specified amount of frames later");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Graphics")) {
            // New
            UIWidgets::CVarCombobox("Menu Theme", "gMenuTheme", uiMenuColors, {
                .color = colorChoice,
                .tooltip = "Change the Color Theme of the Menu Bar."
            });
            switch (CVarGetInteger("gMenuTheme", 0)) {
                case 1:
                    colorChoice = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
                    break;
                case 2:
                    colorChoice = ImVec4(0.24f, 0.31f, 0.71f, 1.0f);
                    break;
                case 3:
                    colorChoice = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
                    break;
                case 4:
                    colorChoice = ImVec4(0.3f, 0.0f, 0.0f, 1.0f);
                    break;
                case 5:
                    colorChoice = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);
                    break;
                case 6:
                    colorChoice = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
                    break;
                case 7:
                    colorChoice = ImVec4(0.0f, 0.3f, 0.0f, 1.0f);
                    break;
                case 8:
                    colorChoice = ImVec4(1.0f, 0.627f, 0.0f, 1.0f);
                    break;
                default:
                    colorChoice = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
            }
        // Old
        #ifndef __APPLE__
            const bool disabled_resolutionSlider = CVarGetInteger("gAdvancedResolution.VerticalResolutionToggle", 0) &&
                                                   CVarGetInteger("gAdvancedResolution.Enabled", 0);
            if (UIWidgets::EnhancementSliderFloat("Internal Resolution: %.1f %%", "##IMul", "gInternalResolution", 0.5f,
                                                  2.0f, "", 1.0f, true, true, disabled_resolutionSlider)) {
                LUS::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(CVarGetFloat("gInternalResolution", 1));
            }
            UIWidgets::Tooltip("Multiplies your output resolution by the value inputted, as a more intensive but effective form of anti-aliasing");
        #endif
            
            if (mAdvancedResolutionSettingsWindow) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                UIWidgets::Spacer(0);
                if (ImGui::Button(GetWindowButtonText("Advanced Resolution", CVarGetInteger("gAdvancedResolutionEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                    mAdvancedResolutionSettingsWindow->ToggleVisibility();
                }
                ImGui::PopStyleColor(1);
                ImGui::PopStyleVar(3);
            }

        #ifndef __WIIU__
            if (UIWidgets::PaddedEnhancementSliderInt("MSAA: %d", "##IMSAA", "gMSAAValue", 1, 8, "", 1, true, true, false)) {
                LUS::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger("gMSAAValue", 1));
            };
            UIWidgets::Tooltip("Activates multi-sample anti-aliasing when above 1x up to 8x for 8 samples for every pixel");
        #endif

            { // FPS Slider
                const int minFps = 20;
                static int maxFps;
                if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                    maxFps = 360;
                } else {
                    maxFps = LUS::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
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
                if (CVarGetInteger("gMatchRefreshRate", 0)) {
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

                if (CVarGetInteger("gMatchRefreshRate", 0)) {
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
                CVarSetInteger("gInterpolationFPS", currentFps);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            #else
                bool matchingRefreshRate =
                    CVarGetInteger("gMatchRefreshRate", 0) && LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() != LUS::WindowBackend::DX11;
                UIWidgets::PaddedEnhancementSliderInt(
                    (currentFps == 20) ? "FPS: Original (20)" : "FPS: %d",
                    "##FPSInterpolation", "gInterpolationFPS", minFps, maxFps, "", 20, true, true, false, matchingRefreshRate);
            #endif
                if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                    UIWidgets::Tooltip(
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is purely "
                        "visual and does not impact game logic, execution of glitches etc.\n\n"
                        "A higher target FPS than your monitor's refresh rate will waste resources, and might give a worse result."
                    );
                } else {
                    UIWidgets::Tooltip(
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is purely "
                        "visual and does not impact game logic, execution of glitches etc."
                    );
                }
            } // END FPS Slider

            if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                UIWidgets::Spacer(0);
                if (ImGui::Button("Match Refresh Rate")) {
                    int hz = LUS::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
                    if (hz >= 20 && hz <= 360) {
                        CVarSetInteger("gInterpolationFPS", hz);
                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                }
            } else {
                UIWidgets::PaddedEnhancementCheckbox("Match Refresh Rate", "gMatchRefreshRate", true, false);
            }
            UIWidgets::Tooltip("Matches interpolation value to the current game's window refresh rate");

            if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                UIWidgets::PaddedEnhancementSliderInt(CVarGetInteger("gExtraLatencyThreshold", 80) == 0 ? "Jitter fix: Off" : "Jitter fix: >= %d FPS",
                    "##ExtraLatencyThreshold", "gExtraLatencyThreshold", 0, 360, "", 80, true, true, false);
                UIWidgets::Tooltip("When Interpolation FPS setting is at least this threshold, add one frame of input lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter. This setting allows the CPU to work on one frame while GPU works on the previous frame.\nThis setting should be used when your computer is too slow to do CPU + GPU work in time.");
            }

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
            ImGui::Text("ImGui Menu Scale");
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, "(Experimental)");
            if (UIWidgets::EnhancementCombobox("gImGuiScale", imguiScaleOptions, 1)) {
                OTRGlobals::Instance->ScaleImGui();
            }
            UIWidgets::Tooltip("Changes the scaling of the ImGui menu elements.");

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
            
            static std::unordered_map<LUS::WindowBackend, const char*> windowBackendNames = {
                { LUS::WindowBackend::DX11, "DirectX" },
                { LUS::WindowBackend::SDL_OPENGL, "OpenGL"},
                { LUS::WindowBackend::SDL_METAL, "Metal" },
                { LUS::WindowBackend::GX2, "GX2"}
            };

            ImGui::Text("Renderer API (Needs reload)");
            LUS::WindowBackend runningWindowBackend = LUS::Context::GetInstance()->GetWindow()->GetWindowBackend();
            LUS::WindowBackend configWindowBackend;
            int configWindowBackendId = LUS::Context::GetInstance()->GetConfig()->GetInt("Window.Backend.Id", -1);
            if (configWindowBackendId != -1 && configWindowBackendId < static_cast<int>(LUS::WindowBackend::BACKEND_COUNT)) {
                configWindowBackend = static_cast<LUS::WindowBackend>(configWindowBackendId);
            } else {
                configWindowBackend = runningWindowBackend;
            }

            if (LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size() <= 1) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::BeginCombo("##RApi", windowBackendNames[configWindowBackend])) {
                for (size_t i = 0; i < LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size(); i++) {
                    auto backend = LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->data()[i];
                    if (ImGui::Selectable(windowBackendNames[backend], backend == configWindowBackend)) {
                        LUS::Context::GetInstance()->GetConfig()->SetInt("Window.Backend.Id", static_cast<int>(backend));
                        LUS::Context::GetInstance()->GetConfig()->SetString("Window.Backend.Name",
                                                                            windowBackendNames[backend]);
                        LUS::Context::GetInstance()->GetConfig()->Save();
                    }
                }
                ImGui::EndCombo();
            }
            if (LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size() <= 1) {
                UIWidgets::ReEnableComponent("");
            }

            if (LUS::Context::GetInstance()->GetWindow()->CanDisableVerticalSync()) {
                UIWidgets::PaddedEnhancementCheckbox("Enable Vsync", "gVsyncEnabled", true, false);
            }

            if (LUS::Context::GetInstance()->GetWindow()->SupportsWindowedFullscreen()) {
                UIWidgets::PaddedEnhancementCheckbox("Windowed fullscreen", "gSdlWindowedFullscreen", true, false);
            }

            if (LUS::Context::GetInstance()->GetWindow()->GetGui()->SupportsViewports()) {
                UIWidgets::PaddedEnhancementCheckbox("Allow multi-windows", "gEnableMultiViewports", true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
                UIWidgets::Tooltip("Allows windows to be able to be dragged off of the main game window. Requires a reload to take effect.");
            }

            // If more filters are added to LUS, make sure to add them to the filters list here
            ImGui::Text("Texture Filter (Needs reload)");

            UIWidgets::EnhancementCombobox("gTextureFilter", filters, FILTER_THREE_POINT);

            UIWidgets::Spacer(0);

            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->DrawSettings();

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Languages")) {
            UIWidgets::PaddedEnhancementCheckbox("Translate Title Screen", "gTitleScreenTranslation");
            if (UIWidgets::EnhancementRadioButton("English", "gLanguages", LANGUAGE_ENG)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("German", "gLanguages", LANGUAGE_GER)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            if (UIWidgets::EnhancementRadioButton("French", "gLanguages", LANGUAGE_FRA)) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            ImGui::EndMenu();
        }
        
        UIWidgets::Spacer(0);
        
        if (ImGui::BeginMenu("Accessibility")) {
        #if defined(_WIN32) || defined(__APPLE__)
            UIWidgets::PaddedEnhancementCheckbox("Text to Speech", "gA11yTTS");
            UIWidgets::Tooltip("Enables text to speech for in game dialog");
        #endif
            UIWidgets::PaddedEnhancementCheckbox("Disable Idle Camera Re-Centering", "gA11yDisableIdleCam");
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
        ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));

        DrawPresetSelector(PRESET_TYPE_ENHANCEMENTS);

        UIWidgets::PaddedSeparator();

        if (ImGui::BeginMenu("Gameplay"))
        {
            UIWidgets::Button("Time Savers", {
               .color = colorChoice,
               .tooltip = "Quality of life Time Saving options.",
            });

            if (ImGui::BeginMenu("Time Savers"))
            {
                // new
                ImGui::BeginTable("Time Savers", 2);
                ImGui::TableNextColumn();
                UIWidgets::CVarSliderInt("Text Speed", "gTextSpeed", 1, 5, 1, {
                    .color = colorChoice,
                    .tooltip = "The speed at which text appears in dialogue boxes.",
                    .format = "%dx",
                });
                UIWidgets::CVarCheckbox("Skip Text", "gSkipText", {
                    .color = colorChoice,
                    .tooltip = "Holding down B skips text",
                });
                UIWidgets::CVarSliderInt("King Zora Speed", "gMweepSpeed", 1, 5, 1, {
                    .color = colorChoice,
                    .tooltip = "Increases the distance per Mweep!",
                    .format = "%dx",
                });
                UIWidgets::CVarSliderInt("Biggoron Forge Time", "gForgeTime", 0, 3, 3, {
                    .color = colorChoice,
                    .tooltip = "Allows you to change the number of days it takes for Biggoron to forge the Biggoron Sword",
                    .format = "%d days"
                });
                UIWidgets::CVarSliderInt("Vine/Ladder Climb speed", "gClimbSpeed", 0, 12, 0, {
                    .color = colorChoice,
                    .tooltip = "Makes Link climb up climbable surfaces faster.",
                    .format = "%dx",
                });
                UIWidgets::CVarSliderInt("Block pushing speed", "gFasterBlockPush", 0, 5, 0, {
                    .color = colorChoice,
                    .tooltip = "Link moves Blocks faster.",
                    .format = "%dx",
                });
                UIWidgets::CVarCheckbox("Faster Heavy Block Lift", "gFasterHeavyBlockLift", {
                    .color = colorChoice,
                    .tooltip = "Speeds up lifting silver rocks and obelisks",
                });
                UIWidgets::CVarCheckbox("No Skulltula Freeze", "gSkulltulaFreeze", {
                    .color = colorChoice,
                    .tooltip = "Stops the game from freezing the player when picking up Gold Skulltulas",
                });
                UIWidgets::CVarCheckbox("Fast Chests", "gFastChests", {
                    .color = colorChoice,
                    .tooltip = "Kick open every chest",
                });
                UIWidgets::CVarCombobox("Chest size & texture matches contents", "gChestSizeAndTextureMatchesContents", chestStyleMatchesContentsOptions, {
                    .color = colorChoice,
                    .tooltip =
                        "Chest sizes and textures are changed to help identify the item inside.\n"
                        " - Major items: Large gold chests\n"
                        " - Lesser items: Large brown chests\n"
                        " - Junk items: Small brown chests\n"
                        " - Small keys: Small silver chest\n"
                        " - Boss keys: Vanilla size and texture\n"
                        " - Skulltula Tokens: Small skulltula chest\n",
                    .defaultIndex = CSMC_DISABLED,
                });
                if (CVarGetInteger("gChestSizeAndTextureMatchesContents", CSMC_DISABLED) != CSMC_DISABLED) {
                    UIWidgets::CVarCheckbox("Chests of Agony", "gChestSizeDependsStoneOfAgony", {
                        .color = colorChoice,
                        .tooltip = "Only change the size/texture of chests if you have the Stone of Agony.",
                    });
                }

                ImGui::TableNextColumn();
                UIWidgets::CVarCheckbox("Link as default file name", "gLinkDefaultName", {
                    .color = colorChoice,
                    .tooltip = "Allows you to have \"Link\" as a premade file name",
                });
                UIWidgets::CVarCheckbox("No Forced Navi", "gNoForcedNavi", {
                    .color = colorChoice,
                    .tooltip = "Prevent forced Navi conversations",
                });
                UIWidgets::CVarCheckbox("Skip Pickup Messages", "gFastDrops", {
                    .color = colorChoice,
                    .tooltip = "Only change the size/texture of chests if you have the Stone of Agony.",
                });
                UIWidgets::CVarCheckbox("Ask to Equip New Items", "gAskToEquip", {
                    .color = colorChoice,
                    .tooltip = "Adds a prompt to equip newly-obtained swords, shields and tunics",
                });
                UIWidgets::CVarCheckbox("Better Owl", "gBetterOwl", {
                    .color = colorChoice,
                    .tooltip = "The default response to Kaepora Gaebora is always that you understood what he said",
                });
                UIWidgets::CVarCheckbox("Fast Ocarina Playback", "gFastOcarinaPlayback", {
                    .color = colorChoice,
                    .tooltip = "Skip the part where the Ocarina playback is called when you play a song",
                });
                bool forceSkipScarecrow = IS_RANDO &&
                    OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SKIP_SCARECROWS_SONG);
                static const char* forceSkipScarecrowText =
                    "This setting is forcefully enabled because a savefile\nwith \"Skip Scarecrow Song\" is loaded";
                UIWidgets::CVarCheckbox("Skip Scarecrow Song", "gSkipScarecrow", {
                    .color = colorChoice,
                    .tooltip = "Pierre appears when Ocarina is pulled out. Requires learning scarecrow song.",
                    .disabled = forceSkipScarecrow,
                    .disabledTooltip = forceSkipScarecrowText,
                });
                UIWidgets::CVarCheckbox("Remember Save Location", "gRememberSaveLocation", {
                    .color = colorChoice,
                    .tooltip = "When loading a save, places Link at the last entrance he went through.\n"
                                   "This doesn't work if the save was made in a grotto.",
                });
                UIWidgets::CVarCheckbox("Skip Magic Arrow Equip Animation", "gSkipArrowAnimation", {
                    .color = colorChoice,
                    .tooltip = "Skips the animation when equiping Magic Arrows.",
                });
                UIWidgets::CVarCheckbox("Skip save confirmation", "gSkipSaveConfirmation", {
                    .color = colorChoice,
                    .tooltip = "Skip the \"Game saved.\" confirmation screen",
                });
                UIWidgets::CVarCheckbox("Exit Market at Night", "gMarketSneak", {
                    .color = colorChoice,
                    .tooltip = "Allows exiting Hyrule Castle Market Town to Hyrule Field at night by speaking \n"
                                   "to the guard next to the gate.",
                });
                UIWidgets::CVarCheckbox("Faster Farore's Wind", "gFastFarores", {
                    .color = colorChoice,
                    .tooltip = "Greatly decreases cast time of Farore's Wind magic spell.",
                });
                UIWidgets::CVarCheckbox("Nighttime GS Always Spawn", "gNightGSAlwaysSpawn", {
                    .color = colorChoice,
                    .tooltip = "Nighttime Skulltulas will spawn during both day and night.",
                });
                UIWidgets::CVarCheckbox("Dampe Appears All Night", "gDampeAllNight", {
                    .color = colorChoice,
                    .tooltip = "Makes Dampe appear anytime during the night, not just his usual working hours.",
                });
                UIWidgets::CVarCombobox("Time Travel with the Song of Time", "gTimeTravel", timeTravelOptions, {
                    .color = colorChoice,
                    .tooltip =
                        "Allows Link to freely change age by playing the Song of Time.\n"
                        "Time Blocks can still be used properly.\n\n"
                        "Requirements:\n"
                        "- Obtained the Ocarina of Time (depends on selection)\n"
                        "- Obtained the Song of Time\n"
                        "- Obtained the Master Sword\n"
                        "- Not within range of Time Block\n"
                        "- Not within range of Ocarina playing spots",
                    .defaultIndex = 0,
                });
                ImGui::EndTable();
                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Items"))
            {
                UIWidgets::CVarCheckbox("Instant Putaway", "gInstantPutaway", {
                    .color = colorChoice,
                    .tooltip = "Allow Link to put items away without having to wait around",
                });
                UIWidgets::CVarCheckbox("Instant Boomerang Recall", "gFastBoomerang", {
                    .color = colorChoice,
                    .tooltip = "Instantly return the boomerang to Link by pressing its item button while it's in the air",
                });
                UIWidgets::CVarCheckbox("Prevent Dropped Ocarina Inputs", "gDpadNoDropOcarinaInput", {
                    .color = colorChoice,
                    .tooltip = "Prevent dropping inputs when playing the ocarina quickly",
                });
                UIWidgets::CVarCombobox("Bunny Hood Effect", "gMMBunnyHood", bunnyHoodOptions, {
                    .color = colorChoice,
                    .tooltip =
                        "Wearing the Bunny Hood grants a speed increase like in Majora's Mask.\n"
                        "The longer jump option is not accounted for in randomizer logic.\n\n"
                        "Also disables NPC's reactions to wearing the Bunny Hood.",
                    .defaultIndex = BUNNY_HOOD_VANILLA,
                });
                UIWidgets::CVarCheckbox("Mask Select in Inventory", "gMaskSelect", {
                    .color = colorChoice,
                    .tooltip = "After completing the mask trading sub-quest,\n"
                                   "press A and any direction on the mask slot to change masks.",
                });
                UIWidgets::CVarCheckbox("Nuts explode bombs", "gNutsExplodeBombs", {
                    .color = colorChoice,
                    .tooltip = "Makes nuts explode bombs, similar to how they interact with bombchus.\n"
                                   "This does not affect bombflowers.",
                });
                UIWidgets::CVarCheckbox("Equip Multiple Arrows at Once", "gSeparateArrows", {
                    .color = colorChoice,
                    .tooltip = "Allow the bow and magic arrows to be equipped at the same time on different slots",
                });
                UIWidgets::CVarCheckbox("Bow as Child/Slingshot as Adult", "gBowSlingShotAmmoFix", {
                    .color = colorChoice,
                    .tooltip = "Allows child to use bow with arrows.\nAllows adult to use slingshot with seeds.\n\n"
                                   "Requires glitches or 'Timeless Equipment' cheat to equip.",
                });
                UIWidgets::CVarCheckbox("Better Farore's Wind", "gBetterFW", {
                    .color = colorChoice,
                    .tooltip = "Helps FW persist between ages, gives child and adult separate FW points, and can be used in more places.",
                });
                UIWidgets::CVarCheckbox("Static Explosion Radius", "gStaticExplosionRadius", {
                    .color = colorChoice,
                    .tooltip = "Explosions are now a static size, like in Majora's Mask and OoT3D. Makes bombchu hovering much easier.",
                });
                UIWidgets::CVarCheckbox("Prevent Bombchus Forcing First-Person", "gDisableFirstPersonChus", {
                    .color = colorChoice,
                    .tooltip = "Prevent bombchus from forcing the camera into first-person mode when released.",
                });
                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Difficulty Options"))
            {
                ImGui::BeginTable("Difficulty Options", 3);
                ImGui::TableNextColumn();
                UIWidgets::Spacer(0);
                UIWidgets::CVarCombobox("Damage Multiplier", "gDamageMul", allPowers, {
                    .color = colorChoice,
                    .tooltip = "Modifies all sources of damage not affected by other sliders\n"
                               "2x: Can survive all common attacks from the start of the game\n"
                               "4x: Dies in 1 hit to any substantial attack from the start of the game\n"
                               "8x: Can only survive trivial damage from the start of the game\n"
                               "16x: Can survive all common attacks with max health without double defense\n"
                               "32x: Can survive all common attacks with max health and double defense\n"
                               "64x: Can survive trivial damage with max health without double defense\n"
                               "128x: Can survive trivial damage with max health and double defense\n"
                               "256x: Cannot survive damage",
                    .defaultIndex = 0,
                });
                UIWidgets::CVarCombobox("Fall Damage Multiplier", "gFallDamageMul", subPowers, {
                    .color = colorChoice,
                    .tooltip = "Modifies all fall damage\n"
                               "2x: Can survive all fall damage from the start of the game\n"
                               "4x: Can only survive short fall damage from the start of the game\n"
                               "8x: Cannot survive any fall damage from the start of the game\n"
                               "16x: Can survive all fall damage with max health without double defense\n"
                               "32x: Can survive all fall damage with max health and double defense\n"
                               "64x: Can survive short fall damage with double defense\n"
                               "128x: Cannot survive fall damage",
                    .defaultIndex = 0,
                });
                UIWidgets::CVarCombobox("Void Damage Multiplier", "gVoidDamageMul", subSubPowers, {
                    .color = colorChoice,
                    .tooltip = "Modifies damage taken after falling into a void\n"
                               "2x: Can survive void damage from the start of the game\n"
                               "4x: Cannot survive void damage from the start of the game\n"
                               "8x: Can survive void damage twice with max health without double defense\n"
                               "16x: Can survive void damage with max health without double defense\n"
                               "32x: Can survive void damage with max health and double defense\n"
                               "64x: Cannot survive void damage",
                    .defaultIndex = 0,
                });
                UIWidgets::CVarCombobox("Bonk Damage Multiplier", "gBonkDamageMul", bonkDamageValues, {
                    .color = colorChoice,
                    .tooltip = "Modifies damage taken after bonking.",
                    .defaultIndex = BONK_DAMAGE_NONE,
                });
                UIWidgets::CVarCheckbox("Spawn with full health", "gFullHealthSpawn", {
                    .color = colorChoice,
                    .tooltip = "Respawn with full health instead of 3 Hearts",
                });
                UIWidgets::CVarCheckbox("No Random Drops", "gNoRandomDrops", {
                    .color = colorChoice,
                    .tooltip = "Disables random drops, except from the Goron Pot, Dampe, and bosses",
                });
                bool forceEnableBombchuDrops =
                    IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_ENABLE_BOMBCHU_DROPS) == 1;
                static const char* forceEnableBombchuDropsText =
                    "This setting is forcefully enabled because a savefile\nwith \"Enable Bombchu Drops\" is loaded.";
                UIWidgets::CVarCheckbox("Enable Bombchu Drops", "gBombchuDrops", {
                    .color = colorChoice,
                    .tooltip = "Bombchus will sometimes drop in place of bombs",
                    .disabled = forceEnableBombchuDrops,
                    .disabledTooltip = forceEnableBombchuDropsText,
                });
                UIWidgets::CVarCheckbox("Trees Drop Sticks", "gTreeStickDrops", {
                    .color = colorChoice,
                    .tooltip = "Bonking into trees will have a chance to drop up to 3 sticks. Must already have obtained sticks.",
                });
                UIWidgets::CVarCheckbox("No Heart Drops", "gNoHeartDrops", {
                    .color = colorChoice,
                    .tooltip = "Disables heart drops, but not heart placements, like from a Deku Scrub running off\n"
                                   "This simulates Hero Mode from other games in the series",
                });
                UIWidgets::CVarCheckbox("Hyper Bosses", "gHyperBosses", {
                    .color = colorChoice,
                    .tooltip = "All major bosses move and act twice as fast.",
                });
                UIWidgets::CVarCheckbox("Hyper Enemies", "gHyperEnemies", {
                    .color = colorChoice,
                    .tooltip = "All regular enemies and mini-bosses move and act twice as fast.",
                });
                UIWidgets::CVarCheckbox("Always Win Goron Pot", "gGoronPot", {
                    .color = colorChoice,
                    .tooltip = "Always get the heart piece/purple rupee from the spinning Goron pot.",
                });
                bool forceEnableDampeWin = IS_RANDO;
                static const char* forceEnableDampeWinText = "This setting is always enabled in randomizer files";
                UIWidgets::CVarCheckbox("Always Win Dampe Digging Game", "gDampeWin", {
                    .color = colorChoice,
                    .tooltip = "Always win the heart piece/purple rupee on the first dig in Dampe's grave digging game, just like in rando.\n"
                               "In a rando file, this is unconditionally enabled",
                    .disabled = forceEnableDampeWin,
                    .disabledTooltip = forceEnableDampeWinText,
                });

                ImGui::TableNextColumn();
                ImGui::Text("Restoration Effects                    ");
                UIWidgets::CVarSliderInt("Change Red Potion Effect", "gRedPercentRestore", 1, 100, 100, {
                    .color = colorChoice,
                    .tooltip = "Changes the amount of health restored by Red Potions.",
                    .format = "%d%",
                });
                UIWidgets::CVarSliderInt("Change Green Potion Effect", "gGreenPercentRestore", 1, 100, 100, {
                    .color = colorChoice,
                    .tooltip = "Changes the amount of mana restored by Green Potions.",
                    .format = "%d%",
                });
                UIWidgets::CVarSliderInt("Change Blue Potion Effect", "gBluePercentRestore", 1, 100, 100, {
                    .color = colorChoice,
                    .tooltip = "Changes the amount of health and mana restored by Blue Potions.",
                    .format = "%d%",
                });
                UIWidgets::CVarSliderInt("Change Milk Effect", "gMilkPercentRestore", 1, 100, 100, {
                    .color = colorChoice,
                    .tooltip = "Changes the amount of health restored by Milk.",
                    .format = "%d%",
                });
                /* Half Milk?
                UIWidgets::CVarSliderInt("Change Blue Potion Effect", "gBluePercentRestore", 1, 100, 100, {
                    .color = colorChoice,
                    .tooltip = "Changes the amount of health and mana restored by Blue Potions.",
                    .format = "%d%",
                });
                */
                UIWidgets::CVarSliderInt("Change Fairy Effect", "gFairyPercentRestore", 1, 100, 100, {
                    .color = colorChoice,
                    .tooltip = "Changes the amount of health restored by Fairies.",
                    .format = "%d%",
                });
                UIWidgets::CVarSliderInt("Change Fairy Revive Effect", "gFairyRevivePercentRestore", 1, 100, 100, {
                    .color = colorChoice,
                    .tooltip = "Changes the amount of health restored by Fairy Revivals.",
                    .format = "%d%",
                });

                ImGui::TableNextColumn();
                ImGui::Text("Mini Games                      ");
                UIWidgets::CVarCheckbox("Shooting Gallery", "gCustomizeShootingGallery", {
                    .color = colorChoice, 
                    .tooltip = "Turn on/off changes to the behavior of the Shooting Galleries.",
                });
                if (CVarGetInteger("gCustomizeShootingGallery", 0)) {
                    UIWidgets::PaddedSeparator();
                    bool disabledShootingGallery = !CVarGetInteger("gCustomizeShootingGallery", 0);
                    static const char* disabledShootingGalleryText = "This option is disabled because \"Shooting Gallery\" is turned off.";
                    UIWidgets::CVarCheckbox("Instant Win", "gInstantShootingGalleryWin", {
                        .color = colorChoice,
                        .tooltip = "Skips the shooting gallery minigame.",
                        .disabled = disabledShootingGallery,
                        .disabledTooltip = disabledShootingGalleryText,
                    });
                    UIWidgets::CVarSliderInt("Child Starting Ammunition", "gChildShootingGalleryAmmunition", 10, 30, 15, {
                        .color = colorChoice,
                        .tooltip = "The ammunition at the start of the shooting gallery minigame as a child.",
                        .disabled = disabledShootingGallery,
                        .disabledTooltip = disabledShootingGalleryText,
                        .format = "%d Seeds",
                    });
                    UIWidgets::CVarSliderInt("Adult Starting Ammunition", "gAdultShootingGalleryAmmunition", 10, 30, 15, {
                        .color = colorChoice,
                        .tooltip = "The ammunition at the start of the shooting gallery minigame as an Adult.",
                        .disabled = disabledShootingGallery,
                        .disabledTooltip = disabledShootingGalleryText,
                        .format = "%d Arrows",
                    });
                    UIWidgets::PaddedSeparator();
                }
                UIWidgets::CVarCheckbox("Bombchu Bowling", "gCustomizeBombchuBowling", {
                    .color = colorChoice, 
                    .tooltip = "Turn on/off changes to the behavior of Bombchu Bowling.",
                });
                if (CVarGetInteger("gCustomizeBombchuBowling", 0)) {
                    UIWidgets::PaddedSeparator();
                    bool disabledBombchuBowling = !CVarGetInteger("gCustomizeBombchuBowling", 0);
                    static const char* disabledBombchuBowlingText = "This option is disabled because \"Bombchu Bowling\" is turned off.";
                    UIWidgets::CVarCheckbox("Remove Small Cucco", "gBombchuBowlingNoSmallCucco", {
                        .color = colorChoice, 
                        .tooltip = "Prevents the small cucco from appearing in the bombchu bowling minigame.",
                        .disabled = disabledBombchuBowling,
                        .disabledTooltip = disabledBombchuBowlingText,
                    });
                    UIWidgets::CVarCheckbox("Remove Big Cucco", "gBombchuBowlingNoBigCucco", {
                        .color = colorChoice, 
                        .tooltip = "Prevents the big cucco from appearing in the bombchu bowling minigame.",
                        .disabled = disabledBombchuBowling,
                        .disabledTooltip = disabledBombchuBowlingText,
                    });
                    UIWidgets::CVarSliderInt("Bombchu Starting Ammunition", "gBombchuBowlingAmmunition", 3, 20, 10, {
                        .color = colorChoice,
                        .tooltip = "The number of bombchus available at the start of the bombchu bowling minigame.",
                        .disabled = disabledBombchuBowling,
                        .disabledTooltip = disabledBombchuBowlingText,
                        .format = "%d Bombchus",
                    });
                    UIWidgets::PaddedSeparator();
                }
                UIWidgets::CVarCheckbox("Fishing", "gCustomizeFishing", {
                    .color = colorChoice, 
                    .tooltip = "Turn on/off changes to the behavior of Adult and Child Fishing.",
                });
                if (CVarGetInteger("gCustomizeFishing", 0)) {
                    UIWidgets::PaddedSeparator();
                    bool disabledFishing = !CVarGetInteger("gCustomizeFishing", 0);
                    static const char* disabledFishingText = "This option is disabled because \"Fishing\" is turned off.";
                    UIWidgets::CVarCheckbox("Instant Fishing", "gInstantFishing", {
                        .color = colorChoice, 
                        .tooltip = "All fish will be caught instantly.",
                        .disabled = disabledFishing,
                        .disabledTooltip = disabledFishingText,
                    });
                    UIWidgets::CVarCheckbox("Guarantee Bite", "gGuaranteeFishingBite", {
                        .color = colorChoice, 
                        .tooltip = "When a line is stable, guarantee a bite. Otherwise use default logic.",
                        .disabled = disabledFishing,
                        .disabledTooltip = disabledFishingText,
                    });
                    UIWidgets::CVarCheckbox("Fish Never Escape", "gFishNeverEscape", {
                        .color = colorChoice, 
                        .tooltip = "Once a hook has been set, fish will never let go while being reeled in.",
                        .disabled = disabledFishing,
                        .disabledTooltip = disabledFishingText,
                    });
                    UIWidgets::CVarSliderInt("Child Minimum Weight", "gChildMinimumWeightFish", 3, 10, 10, {
                        .color = colorChoice,
                        .tooltip = "The minimum weight for the unique fishing reward as a child.",
                        .disabled = disabledFishing,
                        .disabledTooltip = disabledFishingText,
                        .format = "%d",
                    });
                    UIWidgets::CVarSliderInt("Adult Minimum Weight", "gAdultMinimumWeightFish", 6, 13, 13, {
                        .color = colorChoice,
                        .tooltip = "The minimum weight for the unique fishing reward as an Adult.",
                        .disabled = disabledFishing,
                        .disabledTooltip = disabledFishingText,
                        .format = "%d",
                    });
                    UIWidgets::PaddedSeparator();
                }

                ImGui::EndTable();
                ImGui::EndMenu();
            }

            // Old
            UIWidgets::Spacer(0);

            if (ImGui::BeginMenu("Reduced Clutter"))
            {
                UIWidgets::EnhancementCheckbox("Mute Low HP Alarm", "gLowHpAlarm");
                UIWidgets::Tooltip("Disable the low HP beeping sound");
                UIWidgets::PaddedEnhancementCheckbox("Minimal UI", "gMinimalUI", true, false);
                UIWidgets::Tooltip("Hides most of the UI when not needed\nNote: Doesn't activate until after loading a new scene");
                UIWidgets::PaddedEnhancementCheckbox("Disable Navi Call Audio", "gDisableNaviCallAudio", true, false);
                UIWidgets::Tooltip("Disables the voice audio when Navi calls you");
                UIWidgets::PaddedEnhancementCheckbox("Disable Hot/Underwater Warning Text", "gDisableTunicWarningText", true, false);
                UIWidgets::Tooltip("Disables warning text when you don't have on the Goron/Zora Tunic in Hot/Underwater conditions.");

                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            UIWidgets::EnhancementCheckbox("Visual Stone of Agony", "gVisualAgony");
            UIWidgets::Tooltip("Displays an icon and plays a sound when Stone of Agony should be activated, for those without rumble");
            UIWidgets::PaddedEnhancementCheckbox("Assignable Tunics and Boots", "gAssignableTunicsAndBoots", true, false);
            UIWidgets::Tooltip("Allows equipping the tunic and boots to c-buttons");
            UIWidgets::PaddedEnhancementCheckbox("Equipment Toggle", "gEquipmentCanBeRemoved", true, false);
            UIWidgets::Tooltip("Allows equipment to be removed by toggling it off on\nthe equipment subscreen.");
            UIWidgets::PaddedEnhancementCheckbox("Link's Cow in Both Time Periods", "gCowOfTime", true, false);
            UIWidgets::Tooltip("Allows the Lon Lon Ranch obstacle course reward to be shared across time periods");
            UIWidgets::PaddedEnhancementCheckbox("Enable visible guard vision", "gGuardVision", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Enable passage of time on file select", "gTimeFlowFileSelect", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Item counts in messages", "gInjectItemCounts", true, false);
            UIWidgets::Tooltip("Injects item counts in pickup messages, like golden skulltula tokens and heart pieces");
            UIWidgets::PaddedEnhancementCheckbox("Pull grave during the day", "gDayGravePull", true, false);
            UIWidgets::Tooltip("Allows graves to be pulled when child during the day");
            UIWidgets::PaddedEnhancementCheckbox("Dogs follow you everywhere", "gDogFollowsEverywhere", true, false);
            UIWidgets::Tooltip("Allows dogs to follow you anywhere you go, even if you leave the market");
            UIWidgets::PaddedEnhancementCheckbox("Don't require input for Credits sequence", "gNoInputForCredits", true, false);
            UIWidgets::Tooltip("Removes the input requirement on textboxes after defeating Ganon, allowing Credits sequence to continue to progress");

            // Blue Fire Arrows
            bool forceEnableBlueFireArrows = IS_RANDO &&
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BLUE_FIRE_ARROWS);
            static const char* forceEnableBlueFireArrowsText =
                "This setting is forcefully enabled because a savefile\nwith \"Blue Fire Arrows\" is loaded.";
            UIWidgets::PaddedEnhancementCheckbox("Blue Fire Arrows", "gBlueFireArrows", true, false, 
                forceEnableBlueFireArrows, forceEnableBlueFireArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Allows Ice Arrows to melt red ice.\nMay require a room reload if toggled during gameplay.");

            // Sunlight Arrows
            bool forceEnableSunLightArrows = IS_RANDO &&
                OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SUNLIGHT_ARROWS);
            static const char* forceEnableSunLightArrowsText =
                "This setting is forcefully enabled because a savefile\nwith \"Sunlight Arrows\" is loaded.";
            UIWidgets::PaddedEnhancementCheckbox("Sunlight Arrows", "gSunlightArrows", true, false, 
                forceEnableSunLightArrows, forceEnableSunLightArrowsText, UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Allows Light Arrows to activate sun switches.\nMay require a room reload if toggled during gameplay.");

            UIWidgets::PaddedEnhancementCheckbox("Disable Crit wiggle", "gDisableCritWiggle", true, false);
            UIWidgets::Tooltip("Disable random camera wiggle at low health");
            UIWidgets::PaddedEnhancementCheckbox("Enemy Health Bars", "gEnemyHealthBar", true, false);
            UIWidgets::Tooltip("Renders a health bar for enemies when Z-Targeted");

            UIWidgets::PaddedEnhancementCheckbox("Targetable Hookshot Reticle", "gHookshotableReticle", true, false);
            UIWidgets::Tooltip("Use a different color when aiming at hookshotable collision");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Graphics"))
        {
            if (ImGui::BeginMenu("Mods")) {
                if (UIWidgets::PaddedEnhancementCheckbox("Use Alternate Assets", "gAltAssets", false, false)) {
                    // The checkbox will flip the alt asset CVar, but we instead want it to change at the end of the game frame
                    // We toggle it back while setting the flag to update the CVar later
                    CVarSetInteger("gAltAssets", !CVarGetInteger("gAltAssets", 0));
                    ToggleAltAssetsAtEndOfFrame = true;
                }
                UIWidgets::Tooltip("Toggle between standard assets and alternate assets. Usually mods will indicate if this setting has to be used or not.");
                UIWidgets::PaddedEnhancementCheckbox("Disable Bomb Billboarding", "gDisableBombBillboarding", true, false);
                UIWidgets::Tooltip("Disables bombs always rotating to face the camera. To be used in conjunction with mods that want to replace bombs with 3D objects.");
                UIWidgets::PaddedEnhancementCheckbox("Disable Grotto Fixed Rotation", "gDisableGrottoRotation", true, false);
                UIWidgets::Tooltip("Disables grottos rotating with the camera. To be used in conjunction with mods that want to replace grottos with 3D objects.");

                ImGui::EndMenu();
            }
            UIWidgets::PaddedEnhancementCheckbox("Disable LOD", "gDisableLOD", true, false);
            UIWidgets::Tooltip("Turns off the Level of Detail setting, making models use their higher-poly variants at any distance");
            if (UIWidgets::PaddedEnhancementCheckbox("Disable Draw Distance", "gDisableDrawDistance", true, false)) {
                if (CVarGetInteger("gDisableDrawDistance", 0) == 0) {
                    CVarSetInteger("gDisableKokiriDrawDistance", 0);
                }
            }
            UIWidgets::Tooltip("Turns off the objects draw distance, making objects being visible from a longer range");
            if (CVarGetInteger("gDisableDrawDistance", 0) == 1) {
                UIWidgets::PaddedEnhancementCheckbox("Kokiri Draw Distance", "gDisableKokiriDrawDistance", true, false);
                UIWidgets::Tooltip("The Kokiri are mystical beings that fade into view when approached\nEnabling this will remove their draw distance");
            }
            UIWidgets::PaddedEnhancementCheckbox("N64 Mode", "gLowResMode", true, false);
            UIWidgets::Tooltip("Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution");
            UIWidgets::PaddedEnhancementCheckbox("Glitch line-up tick", "gDrawLineupTick", true, false);
            UIWidgets::Tooltip("Displays a tick in the top center of the screen to help with glitch line-ups in SoH, as traditional UI based line-ups do not work outside of 4:3");
            UIWidgets::PaddedEnhancementCheckbox("Enable 3D Dropped items/projectiles", "gNewDrops", true, false);
            UIWidgets::Tooltip("Change most 2D items and projectiles on the overworld to their 3D versions");
            UIWidgets::PaddedEnhancementCheckbox("Disable Black Bar Letterboxes", "gDisableBlackBars", true, false);
            UIWidgets::Tooltip("Disables Black Bar Letterboxes during cutscenes and Z-targeting\nNote: there may be minor visual glitches that were covered up by the black bars\nPlease disable this setting before reporting a bug");
            UIWidgets::PaddedEnhancementCheckbox("Dynamic Wallet Icon", "gDynamicWalletIcon", true, false);
            UIWidgets::Tooltip("Changes the rupee in the wallet icon to match the wallet size you currently have");
            UIWidgets::PaddedEnhancementCheckbox("Always show dungeon entrances", "gAlwaysShowDungeonMinimapIcon", true, false);
            UIWidgets::Tooltip("Always shows dungeon entrance icons on the minimap");
            UIWidgets::PaddedEnhancementCheckbox("Show Gauntlets in First Person", "gFPSGauntlets", true, false);
            UIWidgets::Tooltip("Renders Gauntlets when using the Bow and Hookshot like in OOT3D");
            UIWidgets::Spacer(0);
            if (ImGui::BeginMenu("Animated Link in Pause Menu")) {
                ImGui::Text("Rotation");
                UIWidgets::EnhancementRadioButton("Disabled", "gPauseLiveLinkRotation", 0);
                UIWidgets::EnhancementRadioButton("Rotate Link with D-pad", "gPauseLiveLinkRotation", 1);
                UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the D-pad\nUse D-pad Up or D-pad Down to reset Link's rotation");
                UIWidgets::EnhancementRadioButton("Rotate Link with C-buttons", "gPauseLiveLinkRotation", 2);
                UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the C-buttons\nUse C-Up or C-Down to reset Link's rotation");
                UIWidgets::EnhancementRadioButton("Rotate Link with Right Stick", "gPauseLiveLinkRotation", 3);
                UIWidgets::Tooltip("Allow you to rotate Link on the Equipment menu with the Right Stick\nYou can zoom in by pointing up and reset Link's rotation by pointing down");
                if (CVarGetInteger("gPauseLiveLinkRotation", 0) != 0) {
                    UIWidgets::EnhancementSliderInt("Rotation Speed: %d", "##MinRotationSpeed", "gPauseLiveLinkRotationSpeed", 1, 20, "", 1);
                }
                UIWidgets::PaddedSeparator();
                ImGui::Text("Static loop");
                UIWidgets::EnhancementRadioButton("Disabled", "gPauseLiveLink", 0);
                UIWidgets::EnhancementRadioButton("Idle (standing)", "gPauseLiveLink", 1);
                UIWidgets::EnhancementRadioButton("Idle (look around)", "gPauseLiveLink", 2);
                UIWidgets::EnhancementRadioButton("Idle (belt)", "gPauseLiveLink", 3);
                UIWidgets::EnhancementRadioButton("Idle (shield)", "gPauseLiveLink", 4);
                UIWidgets::EnhancementRadioButton("Idle (test sword)", "gPauseLiveLink", 5);
                UIWidgets::EnhancementRadioButton("Idle (yawn)", "gPauseLiveLink", 6);
                UIWidgets::EnhancementRadioButton("Battle Stance", "gPauseLiveLink", 7);
                UIWidgets::EnhancementRadioButton("Walking (no shield)", "gPauseLiveLink", 8);
                UIWidgets::EnhancementRadioButton("Walking (holding shield)", "gPauseLiveLink", 9);
                UIWidgets::EnhancementRadioButton("Running (no shield)", "gPauseLiveLink", 10);
                UIWidgets::EnhancementRadioButton("Running (holding shield)", "gPauseLiveLink", 11);
                UIWidgets::EnhancementRadioButton("Hand on hip", "gPauseLiveLink", 12);
                UIWidgets::EnhancementRadioButton("Spin attack charge", "gPauseLiveLink", 13);
                UIWidgets::EnhancementRadioButton("Look at hand", "gPauseLiveLink", 14);
                UIWidgets::PaddedSeparator();
                ImGui::Text("Randomize");
                UIWidgets::EnhancementRadioButton("Random", "gPauseLiveLink", 15);
                UIWidgets::Tooltip("Randomize the animation played each time you open the menu");
                UIWidgets::EnhancementRadioButton("Random cycle", "gPauseLiveLink", 16);
                UIWidgets::Tooltip("Randomize the animation played on the menu after a certain time");
                UIWidgets::EnhancementRadioButton("Random cycle (Idle)", "gPauseLiveLink", 17);
                UIWidgets::Tooltip("Randomize the animation played on the menu after a certain time (Idle animations only)");
                if (CVarGetInteger("gPauseLiveLink", 0) >= 16) {
                    UIWidgets::EnhancementSliderInt("Frame to wait: %d", "##MinFrameCount", "gMinFrameCount", 1, 1000, "", 0);
                }

                ImGui::EndMenu();
            }
            UIWidgets::PaddedText("Fix Vanishing Paths", true, false);
            if (UIWidgets::EnhancementCombobox("gSceneSpecificDirtPathFix", zFightingOptions, ZFIGHT_FIX_DISABLED) && gPlayState != NULL) {
                UpdateDirtPathFixState(gPlayState->sceneNum);
            }
            UIWidgets::Tooltip("Disabled: Paths vanish more the higher the resolution (Z-fighting is based on resolution)\n"
                                "Consistent: Certain paths vanish the same way in all resolutions\n"
                                "No Vanish: Paths do not vanish, Link seems to sink in to some paths\n"
                                "This might affect other decal effects\n");
            UIWidgets::PaddedEnhancementSliderInt("Text Spacing: %d", "##TEXTSPACING", "gTextSpacing", 4, 6, "", 6, true, true, true);
            UIWidgets::Tooltip("Space between text characters (useful for HD font textures)");
            UIWidgets::PaddedEnhancementCheckbox("More info in file select", "gFileSelectMoreInfo", true, false);
            UIWidgets::Tooltip("Shows what items you have collected in the file select screen, like in N64 randomizer");
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Fixes"))
        {
            UIWidgets::EnhancementCheckbox("Fix L&R Pause menu", "gUniformLR");
            UIWidgets::Tooltip("Makes the L and R buttons in the pause menu the same color");
            UIWidgets::PaddedEnhancementCheckbox("Fix L&Z Page switch in Pause menu", "gNGCKaleidoSwitcher", true, false);
            UIWidgets::Tooltip("Makes L and R switch pages like on the GameCube\nZ opens the Debug Menu instead");
            UIWidgets::PaddedEnhancementCheckbox("Fix Dungeon entrances", "gFixDungeonMinimapIcon", true, false);
            UIWidgets::Tooltip("Removes the dungeon entrance icon on the top-left corner of the screen when no dungeon is present on the current map");
            UIWidgets::PaddedEnhancementCheckbox("Fix Two Handed idle animations", "gTwoHandedIdle", true, false);
            UIWidgets::Tooltip("Re-enables the two-handed idle animation, a seemingly finished animation that was disabled on accident in the original game");
            UIWidgets::PaddedEnhancementCheckbox("Fix the Gravedigging Tour Glitch", "gGravediggingTourFix", true, false);
            UIWidgets::Tooltip("Fixes a bug where the Gravedigging Tour Heart Piece disappears if the area reloads");
            UIWidgets::PaddedEnhancementCheckbox("Fix Deku Nut upgrade", "gDekuNutUpgradeFix", true, false);
            UIWidgets::Tooltip("Prevents the Forest Stage Deku Nut upgrade from becoming unobtainable after receiving the Poacher's Saw");
            UIWidgets::PaddedEnhancementCheckbox("Fix Navi text HUD position", "gNaviTextFix", true, false);
            UIWidgets::Tooltip("Correctly centers the Navi text prompt on the HUD's C-Up button");
            UIWidgets::PaddedEnhancementCheckbox("Fix Anubis fireballs", "gAnubisFix", true, false);
            UIWidgets::Tooltip("Make Anubis fireballs do fire damage when reflected back at them with the Mirror Shield");
            if (UIWidgets::PaddedEnhancementCheckbox("Fix Megaton Hammer crouch stab", "gCrouchStabHammerFix", true, false)) {
                if (!CVarGetInteger("gCrouchStabHammerFix", 0)) {
                    CVarClear("gCrouchStabFix");
                }
            }
            UIWidgets::Tooltip("Make the Megaton Hammer's crouch stab able to destroy rocks without first swinging it normally");
            if (CVarGetInteger("gCrouchStabHammerFix", 0)) {
                UIWidgets::PaddedEnhancementCheckbox("Remove power crouch stab", "gCrouchStabFix", true, false);
                UIWidgets::Tooltip("Make crouch stabbing always do the same damage as a regular slash");
            }
            UIWidgets::PaddedEnhancementCheckbox("Fix credits timing", "gCreditsFix", true, false);
            UIWidgets::Tooltip("Extend certain credits scenes so the music lines up properly with the visuals");
            UIWidgets::PaddedEnhancementCheckbox("Fix Gerudo Warrior's clothing colors", "gGerudoWarriorClothingFix", true, false);
            UIWidgets::Tooltip("Prevent the Gerudo Warrior's clothes changing color when changing Link's tunic or using bombs in front of her");
            UIWidgets::PaddedEnhancementCheckbox("Fix Camera Drift", "gFixCameraDrift", true, false);
            UIWidgets::Tooltip("Fixes camera slightly drifting to the left when standing still due to a math error");
            UIWidgets::PaddedEnhancementCheckbox("Fix Camera Swing", "gFixCameraSwing", true, false);
            UIWidgets::Tooltip("Fixes camera getting stuck on collision when standing still, also fixes slight shift back in camera when stop moving");
            UIWidgets::PaddedEnhancementCheckbox("Fix Hanging Ledge Swing Rate", "gFixHangingLedgeSwingRate", true, false);
            UIWidgets::Tooltip("Fixes camera swing rate when player falls off a ledge and camera swings around");
            UIWidgets::PaddedEnhancementCheckbox("Fix Missing Jingle after 5 Silver Rupees", "gSilverRupeeJingleExtend", true, false);
            UIWidgets::Tooltip(
                "Adds 5 higher pitches for the Silver Rupee Jingle for the rooms with more than 5 Silver Rupees. "
                "Currently only relevant in Master Quest.");
            if (UIWidgets::PaddedEnhancementCheckbox("Fix out of bounds textures", "gFixTexturesOOB", true, false)) {
                ApplyAuthenticGfxPatches();
            }
            UIWidgets::Tooltip("Fixes authentic out of bounds texture reads, instead loading textures with the correct size");
            UIWidgets::PaddedEnhancementCheckbox("Fix Poacher's Saw Softlock", "gFixSawSoftlock", true, false, CVarGetInteger("gSkipText", 0),
                "This is disabled because it is forced on when Skip Text is enabled.", UIWidgets::CheckboxGraphics::Checkmark);
            UIWidgets::Tooltip("Prevents the Poacher's Saw softlock from mashing through the text, or with Skip Text enabled.");
            UIWidgets::PaddedEnhancementCheckbox("Fix Bush Item Drops", "gBushDropFix", true, false);
            UIWidgets::Tooltip("Fixes the bushes to drop items correctly rather than spawning undefined items.");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Restoration"))
        {
            UIWidgets::EnhancementCheckbox("Authentic Logo Screen", "gAuthenticLogo");
            UIWidgets::Tooltip("Hide the game version and build details and display the authentic model and texture on the boot logo start screen");
            UIWidgets::PaddedEnhancementCheckbox("Red Ganon blood", "gRedGanonBlood", true, false);
            UIWidgets::Tooltip("Restore the original red blood from NTSC 1.0/1.1. Disable for green blood");
            UIWidgets::PaddedEnhancementCheckbox("Fish while hovering", "gHoverFishing", true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows casting the Fishing Rod while using the Hover Boots");
            UIWidgets::PaddedEnhancementCheckbox("N64 Weird Frames", "gN64WeirdFrames", true, false);
            UIWidgets::Tooltip("Restores N64 Weird Frames allowing weirdshots to behave the same as N64");
            UIWidgets::PaddedEnhancementCheckbox("Bombchus out of bounds", "gBombchusOOB", true, false);
            UIWidgets::Tooltip("Allows bombchus to explode out of bounds\nSimilar to GameCube and Wii VC");
            UIWidgets::PaddedEnhancementCheckbox("Quick Putaway", "gQuickPutaway", true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows putting away an item without an animation and performing Putaway Ocarina Items");
            UIWidgets::PaddedEnhancementCheckbox("Restore old Gold Skulltula cutscene", "gGsCutscene", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Quick Bongo Kill", "gQuickBongoKill", true, false);
            UIWidgets::Tooltip("Restore a bug from NTSC 1.0 that allows bypassing Bongo Bongo's intro cutscene to quickly kill him");

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (ImGui::BeginMenu("Extra Modes")) {
            UIWidgets::PaddedText("Mirrored World", true, false);
            if (UIWidgets::EnhancementCombobox("gMirroredWorldMode", mirroredWorldModes, MIRRORED_WORLD_OFF) && gPlayState != NULL) {
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
            UIWidgets::EnhancementCombobox("gRandomizedEnemies", enemyRandomizerModes, ENEMY_RANDOMIZER_OFF);
            UIWidgets::Tooltip(
                "Replaces fixed enemies throughout the game with a random enemy. Bosses, mini-bosses and a few specific regular enemies are excluded.\n"
                "Enemies that need more than Deku Nuts + either Deku Sticks or a sword to kill are excluded from spawning in \"clear enemy\" rooms.\n\n"
                "- Random: Enemies are randomized every time you load a room\n"
                "- Random (Seeded): Enemies are randomized based on the current randomizer seed/file\n"
            );

            UIWidgets::PaddedEnhancementCheckbox("Ivan the Fairy (Coop Mode)", "gIvanCoopModeEnabled", true, false);
            UIWidgets::Tooltip("Enables Ivan the Fairy upon the next map change. Player 2 can control Ivan and "
                                "press the C-Buttons to use items and mess with Player 1!");

            UIWidgets::PaddedEnhancementCheckbox("Rupee Dash Mode", "gRupeeDash", true, false);
            UIWidgets::Tooltip("Rupees reduced over time, Link suffers damage when the count hits 0.");

            if (CVarGetInteger("gRupeeDash", 0)) {
                UIWidgets::PaddedEnhancementSliderInt(
                    "Rupee Dash Interval: %d", "##DashInterval", "gDashInterval", 3, 5, "", 5, true, true, false,
                    !CVarGetInteger("gRupeeDash", 0),
                    "This option is disabled because \"Rupee Dash Mode\" is turned off");
                UIWidgets::Tooltip("Interval between Rupee reduction in Rupee Dash Mode");
            }

            UIWidgets::PaddedEnhancementCheckbox("Shadow Tag Mode", "gShadowTag", true, false);
            UIWidgets::Tooltip("A wallmaster follows Link everywhere, don't get caught!");

            UIWidgets::Spacer(0);

            UIWidgets::PaddedEnhancementCheckbox("Additional Traps", "gAddTraps.enabled", true, false);
            UIWidgets::Tooltip("Enables additional Trap variants.");

            if (CVarGetInteger("gAddTraps.enabled", 0)) {
                UIWidgets::PaddedSeparator();
                if (ImGui::BeginMenu("Trap Options")) {
                    ImGui::Text("Tier 1 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Freeze Traps", "gAddTraps.Ice", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Burn Traps", "gAddTraps.Burn", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Shock Traps", "gAddTraps.Shock", true, false);

                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Tier 2 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Knockback Traps", "gAddTraps.Knock", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Speed Traps", "gAddTraps.Speed", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Bomb Traps", "gAddTraps.Bomb", true, false);

                    UIWidgets::PaddedSeparator();
                    ImGui::Text("Tier 3 Traps:");
                    UIWidgets::Spacer(0);
                    UIWidgets::PaddedEnhancementCheckbox("Void Traps", "gAddTraps.Void", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Ammo Traps", "gAddTraps.Ammo", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Death Traps", "gAddTraps.Kill", true, false);
                    UIWidgets::PaddedEnhancementCheckbox("Teleport Traps", "gAddTraps.Tele", true, false);

                    ImGui::EndMenu();
                }
            }

            ImGui::EndMenu();
        }

        UIWidgets::PaddedSeparator(false, true);

        // Autosave enum value of 1 is the default in presets and the old checkbox "on" state for backwards compatibility
        UIWidgets::PaddedText("Autosave", false, true);
        UIWidgets::EnhancementCombobox("gAutosave", autosaveLabels, AUTOSAVE_OFF);
        UIWidgets::Tooltip("Automatically save the game when changing locations and/or obtaining items\n"
            "Major items exclude rupees and health/magic/ammo refills (but include bombchus unless bombchu drops are enabled)");

        UIWidgets::PaddedSeparator(true, true, 2.0f, 2.0f);

        ImGui::EndDisabled();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));

        if (mCosmeticsEditorWindow) {
            if (ImGui::Button(GetWindowButtonText("Cosmetics Editor", CVarGetInteger("gCosmeticsEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mCosmeticsEditorWindow->ToggleVisibility();
            }
        }

        if (mAudioEditorWindow) {
            if (ImGui::Button(GetWindowButtonText("Audio Editor", CVarGetInteger("gAudioEditor.WindowOpen", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mAudioEditorWindow->ToggleVisibility();
            }
        }

        if (mGameplayStatsWindow) {
            if (ImGui::Button(GetWindowButtonText("Gameplay Stats", CVarGetInteger("gGameplayStatsEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mGameplayStatsWindow->ToggleVisibility();
            }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        #ifdef __SWITCH__
        UIWidgets::Spacer(0);
        ImGui::Text("Switch performance mode");
        if (UIWidgets::EnhancementCombobox("gSwitchPerfMode", SWITCH_CPU_PROFILES, (int)LUS::SwitchProfiles::STOCK)) {
            SPDLOG_INFO("Profile:: %s", SWITCH_CPU_PROFILES[CVarGetInteger("gSwitchPerfMode", (int)LUS::SwitchProfiles::STOCK)]);
            LUS::Switch::ApplyOverclock();
        }
        #endif

        ImGui::EndMenu();
    }
}

void DrawCheatsMenu() {
    if (ImGui::BeginMenu("Cheats"))
    {
        ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
        ImGui::BeginTable("##cheatsMenu", 2, ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextColumn();
        UIWidgets::Spacer(2.0f);
        ImGui::Text("Inventory:");
        UIWidgets::PaddedSeparator();

        UIWidgets::PaddedEnhancementCheckbox("Super Tunic", "gSuperTunic", true, false);
        UIWidgets::Tooltip("Makes every tunic have the effects of every other tunic");
        UIWidgets::PaddedEnhancementCheckbox("Easy ISG", "gEzISG", true, false);
        UIWidgets::Tooltip("Passive Infinite Sword Glitch\nIt makes your sword's swing effect and hitbox stay active indefinitely");
        UIWidgets::PaddedEnhancementCheckbox("Easy QPA", "gEzQPA", true, false);
        UIWidgets::Tooltip("Gives you the glitched damage value of the quick put away glitch.");
        UIWidgets::PaddedEnhancementCheckbox("Timeless Equipment", "gTimelessEquipment", true, false);
        UIWidgets::Tooltip("Allows any item to be equipped, regardless of age\nAlso allows Child to use Adult strength upgrades");
        UIWidgets::PaddedEnhancementCheckbox("Unrestricted Items", "gNoRestrictItems", true, false);
        UIWidgets::Tooltip("Allows you to use any item at any location");
        UIWidgets::PaddedEnhancementCheckbox("Fireproof Deku Shield", "gFireproofDekuShield", true, false);
        UIWidgets::Tooltip("Prevents the Deku Shield from burning on contact with fire");
        UIWidgets::PaddedEnhancementCheckbox("Shield with Two-Handed Weapons", "gShieldTwoHanded", true, false);
        UIWidgets::Tooltip("This allows you to put up your shield with any two-handed weapon in hand except for Deku Sticks");
        UIWidgets::Spacer(2.0f);
        ImGui::Text("Deku Sticks:");
        UIWidgets::EnhancementCombobox("gDekuStickCheat", DekuStickCheat, DEKU_STICK_NORMAL);
        UIWidgets::Spacer(2.0f);
        UIWidgets::EnhancementSliderFloat("Bomb Timer Multiplier: %.2fx", "##gBombTimerMultiplier", "gBombTimerMultiplier", 0.1f, 5.0f, "", 1.0f, false);
        UIWidgets::PaddedEnhancementCheckbox("Hookshot Everything", "gHookshotEverything", true, false);
        UIWidgets::Tooltip("Makes every surface in the game hookshot-able");
        UIWidgets::Spacer(0);
        UIWidgets::EnhancementSliderFloat("Hookshot Reach Multiplier: %.2fx", "##gCheatHookshotReachMultiplier", "gCheatHookshotReachMultiplier", 1.0f, 5.0f, "", 1.0f, false);
        UIWidgets::Spacer(2.0f);
        if (ImGui::Button("Change Age")) {
            CVarSetInteger("gSwitchAge", 1);
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
            UIWidgets::EnhancementCheckbox("Money", "gInfiniteMoney");
            UIWidgets::PaddedEnhancementCheckbox("Health", "gInfiniteHealth", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Ammo", "gInfiniteAmmo", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Magic", "gInfiniteMagic", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Nayru's Love", "gInfiniteNayru", true, false);
            UIWidgets::PaddedEnhancementCheckbox("Epona Boost", "gInfiniteEpona", true, false);

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
            if (UIWidgets::PaddedEnhancementCheckbox("I promise I have read the warning", "gSaveStatePromise", true,
                                                     false)) {
                CVarSetInteger("gSaveStatesEnabled", 0);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
            }
            if (CVarGetInteger("gSaveStatePromise", 0) == 1) {
                UIWidgets::PaddedEnhancementCheckbox("I understand, enable save states", "gSaveStatesEnabled", true,
                                                     false);
                UIWidgets::Tooltip("F5 to save, F6 to change slots, F7 to load");
            }

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(2.0f);
        ImGui::Text("Behavior:");
        UIWidgets::PaddedSeparator();

        UIWidgets::PaddedEnhancementCheckbox("No Clip", "gNoClip", true, false);
        UIWidgets::Tooltip("Allows you to walk through walls");
        UIWidgets::PaddedEnhancementCheckbox("Climb Everything", "gClimbEverything", true, false);
        UIWidgets::Tooltip("Makes every surface in the game climbable");
        UIWidgets::PaddedEnhancementCheckbox("Moon Jump on L", "gMoonJumpOnL", true, false);
        UIWidgets::Tooltip("Holding L makes you float into the air");
        UIWidgets::PaddedEnhancementCheckbox("Easy Frame Advancing", "gCheatEasyPauseBufferEnabled", true, false);
        UIWidgets::Tooltip("Continue holding START button when unpausing to only advance a single frame and then re-pause");
        const bool bEasyFrameAdvanceEnabled = CVarGetInteger("gCheatEasyPauseBufferEnabled", 0);
        UIWidgets::PaddedEnhancementCheckbox("Easy Input Buffering", "gCheatEasyInputBufferingEnabled", true, false, bEasyFrameAdvanceEnabled, "Forced enabled when Easy Frame Advancing is enabled", UIWidgets::CheckboxGraphics::Checkmark);
        UIWidgets::Tooltip("Inputs that are held down while the Subscreen is closing will be pressed when the game is resumed");
        UIWidgets::PaddedEnhancementCheckbox("Drops Don't Despawn", "gDropsDontDie", true, false);
        UIWidgets::Tooltip("Drops from enemies, grass, etc. don't disappear after a set amount of time");
        UIWidgets::PaddedEnhancementCheckbox("Fish Don't despawn", "gNoFishDespawn", true, false);
        UIWidgets::Tooltip("Prevents fish from automatically despawning after a while when dropped");
        UIWidgets::PaddedEnhancementCheckbox("Bugs Don't despawn", "gNoBugsDespawn", true, false);
        UIWidgets::Tooltip("Prevents bugs from automatically despawning after a while when dropped");
        UIWidgets::PaddedEnhancementCheckbox("Freeze Time", "gFreezeTime", true, false);
        UIWidgets::Tooltip("Freezes the time of day");
        UIWidgets::PaddedEnhancementCheckbox("Time Sync", "gTimeSync", true, false);
        UIWidgets::Tooltip("This syncs the ingame time with the real world time");
        UIWidgets::PaddedEnhancementCheckbox("No ReDead/Gibdo Freeze", "gNoRedeadFreeze", true, false);
        UIWidgets::Tooltip("Prevents ReDeads and Gibdos from being able to freeze you with their scream");
        {
            static int32_t betaQuestEnabled = CVarGetInteger("gEnableBetaQuest", 0);
            static int32_t lastBetaQuestEnabled = betaQuestEnabled;
            static int32_t betaQuestWorld = CVarGetInteger("gBetaQuestWorld", 0xFFEF);
            static int32_t lastBetaQuestWorld = betaQuestWorld;

            if (!isBetaQuestEnabled) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }

            UIWidgets::PaddedEnhancementCheckbox("Enable Beta Quest", "gEnableBetaQuest", true, false);
            UIWidgets::Tooltip("Turns on OoT Beta Quest. *WARNING* This will reset your game.");
            betaQuestEnabled = CVarGetInteger("gEnableBetaQuest", 0);
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
                CVarClear("gBetaQuestWorld");
            }
            if (betaQuestEnabled != lastBetaQuestEnabled || betaQuestWorld != lastBetaQuestWorld)
            {
                // Reset the game if the beta quest state or world changed because beta quest happens on redirecting the title screen cutscene.
                lastBetaQuestEnabled = betaQuestEnabled;
                lastBetaQuestWorld = betaQuestWorld;
                CVarSetInteger("gEnableBetaQuest", betaQuestEnabled);
                CVarSetInteger("gBetaQuestWorld", betaQuestWorld);

                std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
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

extern std::shared_ptr<LUS::GuiWindow> mStatsWindow;
extern std::shared_ptr<LUS::GuiWindow> mConsoleWindow;
extern std::shared_ptr<SaveEditorWindow> mSaveEditorWindow;
extern std::shared_ptr<ColViewerWindow> mColViewerWindow;
extern std::shared_ptr<ActorViewerWindow> mActorViewerWindow;
extern std::shared_ptr<DLViewerWindow> mDLViewerWindow;
extern std::shared_ptr<ValueViewerWindow> mValueViewerWindow;

void DrawDeveloperToolsMenu() {
    if (ImGui::BeginMenu("Developer Tools")) {
        ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));

        UIWidgets::EnhancementCheckbox("OoT Debug Mode", "gDebugEnabled");
        UIWidgets::Tooltip("Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right, and open the debug menu with L on the pause screen");
        if (CVarGetInteger("gDebugEnabled", 0)) {
            UIWidgets::EnhancementCheckbox("OoT Registry Editor", "gRegEditEnabled");
            UIWidgets::Tooltip("Enables the registry editor");
            ImGui::Text("Debug Save File Mode:");
            UIWidgets::EnhancementCombobox("gDebugSaveFileMode", DebugSaveFileModes, 1);
            UIWidgets::Tooltip(
                "Changes the behaviour of debug file select creation (creating a save file on slot 1 with debug mode on)\n"
                "- Off: The debug save file will be a normal savefile\n"
                "- Vanilla: The debug save file will be the debug save file from the original game\n"
                "- Maxed: The debug save file will be a save file with all of the items & upgrades"
            );
        }
        UIWidgets::PaddedEnhancementCheckbox("OoT Skulltula Debug", "gSkulltulaDebugEnabled", true, false);
        UIWidgets::Tooltip("Enables Skulltula Debug, when moving the cursor in the menu above various map icons (boss key, compass, map screen locations, etc) will set the GS bits in that area.\nUSE WITH CAUTION AS IT DOES NOT UPDATE THE GS COUNT.");
        UIWidgets::PaddedEnhancementCheckbox("Fast File Select", "gSkipLogoTitle", true, false);
        UIWidgets::Tooltip("Load the game to the selected menu or file\n\"Zelda Map Select\" require debug mode else you will fallback to File choose menu\nUsing a file number that don't have save will create a save file only if you toggle on \"Create a new save if none ?\" else it will bring you to the File choose menu");
        if (CVarGetInteger("gSkipLogoTitle", 0)) {
            ImGui::Text("Loading:");
            UIWidgets::EnhancementCombobox("gSaveFileID", FastFileSelect, 0);
        };
        UIWidgets::PaddedEnhancementCheckbox("Better Debug Warp Screen", "gBetterDebugWarpScreen", true, false);
        UIWidgets::Tooltip("Optimized debug warp screen, with the added ability to chose entrances and time of day");
        UIWidgets::PaddedEnhancementCheckbox("Debug Warp Screen Translation", "gDebugWarpScreenTranslation", true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
        UIWidgets::Tooltip("Translate the Debug Warp Screen based on the game language");
        if (gPlayState != NULL) {
            UIWidgets::PaddedSeparator();
            ImGui::Checkbox("Frame Advance##frameAdvance", (bool*)&gPlayState->frameAdvCtx.enabled);
            if (gPlayState->frameAdvCtx.enabled) {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0,0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
                if (ImGui::Button("Advance 1", ImVec2(ImGui::GetContentRegionAvail().x / 2.0f, 0.0f))) {
                    CVarSetInteger("gFrameAdvance", 1);
                }
                ImGui::SameLine();
                ImGui::Button("Advance (Hold)");
                if (ImGui::IsItemActive()) {
                    CVarSetInteger("gFrameAdvance", 1);
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
            if (ImGui::Button(GetWindowButtonText("Stats", CVarGetInteger("gStatsEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mStatsWindow->ToggleVisibility();
            }
            UIWidgets::Tooltip("Shows the stats window, with your FPS and frametimes, and the OS you're playing on");
        }
        UIWidgets::Spacer(0);
        if (mConsoleWindow) {
            if (ImGui::Button(GetWindowButtonText("Console", CVarGetInteger("gConsoleEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mConsoleWindow->ToggleVisibility();
            }
            UIWidgets::Tooltip("Enables the console window, allowing you to input commands, type help for some examples");
        }
        UIWidgets::Spacer(0);
        if (mSaveEditorWindow) {
            if (ImGui::Button(GetWindowButtonText("Save Editor", CVarGetInteger("gSaveEditorEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mSaveEditorWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mColViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Collision Viewer", CVarGetInteger("gCollisionViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mColViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mActorViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Actor Viewer", CVarGetInteger("gActorViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mActorViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mDLViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Display List Viewer", CVarGetInteger("gDLViewerEnabled", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mDLViewerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mValueViewerWindow) {
            if (ImGui::Button(GetWindowButtonText("Value Viewer", CVarGetInteger("gValueViewer.WindowOpen", 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mValueViewerWindow->ToggleVisibility();
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
        static std::string ip = CVarGetString("gRemote.IP", "127.0.0.1");
        static uint16_t port = CVarGetInteger("gRemote.Port", 43384);
        bool isFormValid = !isStringEmpty(CVarGetString("gRemote.IP", "127.0.0.1")) && port > 1024 && port < 65535;

        const char* remoteOptions[2] = { "Sail", "Crowd Control"};

        ImGui::BeginDisabled(GameInteractor::Instance->isRemoteInteractorEnabled);
        ImGui::Text("Remote Interaction Scheme");
        if (UIWidgets::EnhancementCombobox("gRemote.Scheme", remoteOptions, GI_SCHEME_SAIL)) {
            switch (CVarGetInteger("gRemote.Scheme", GI_SCHEME_SAIL)) {
                case GI_SCHEME_SAIL:
                case GI_SCHEME_CROWD_CONTROL:
                    CVarSetString("gRemote.IP", "127.0.0.1");
                    CVarSetInteger("gRemote.Port", 43384);
                    ip = "127.0.0.1";
                    port = 43384;
                    break;
            }
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }
        switch (CVarGetInteger("gRemote.Scheme", GI_SCHEME_SAIL)) {
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
            CVarSetString("gRemote.IP", ip.c_str());
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(ImGui::GetFontSize() * 5);
        if (ImGui::InputScalar("##gRemote.Port", ImGuiDataType_U16, &port)) {
            CVarSetInteger("gRemote.Port", port);
            LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
        }

        ImGui::PopItemWidth();
        ImGui::EndDisabled();

        ImGui::Spacing();

        ImGui::BeginDisabled(!isFormValid);
        const char* buttonLabel = GameInteractor::Instance->isRemoteInteractorEnabled ? "Disable" : "Enable";
        if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
            if (GameInteractor::Instance->isRemoteInteractorEnabled) {
                CVarSetInteger("gRemote.Enabled", 0);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                switch (CVarGetInteger("gRemote.Scheme", GI_SCHEME_SAIL)) {
                    case GI_SCHEME_SAIL:
                        GameInteractorSail::Instance->Disable();
                        break;
                    case GI_SCHEME_CROWD_CONTROL:
                        CrowdControl::Instance->Disable();
                        break;
                }
            } else {
                CVarSetInteger("gRemote.Enabled", 1);
                LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                switch (CVarGetInteger("gRemote.Scheme", GI_SCHEME_SAIL)) {
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
            if (ImGui::Button(GetWindowButtonText("Randomizer Settings", CVarGetInteger("gRandomizerSettingsEnabled", 0)).c_str(), buttonSize)) {
                mRandomizerSettingsWindow->ToggleVisibility();
            }
        }

        UIWidgets::Spacer(0);
        if (mItemTrackerWindow) {
            if (ImGui::Button(GetWindowButtonText("Item Tracker", CVarGetInteger("gItemTrackerEnabled", 0)).c_str(), buttonSize)) {
                mItemTrackerWindow->ToggleVisibility();
            }
        }

        UIWidgets::Spacer(0);
        if (mItemTrackerSettingsWindow) {
            if (ImGui::Button(GetWindowButtonText("Item Tracker Settings", CVarGetInteger("gItemTrackerSettingsEnabled", 0)).c_str(), buttonSize)) {
                mItemTrackerSettingsWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mEntranceTrackerWindow) {
            if (ImGui::Button(GetWindowButtonText("Entrance Tracker", CVarGetInteger("gEntranceTrackerEnabled", 0)).c_str(), buttonSize)) {
                mEntranceTrackerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mCheckTrackerWindow) {
            if (ImGui::Button(GetWindowButtonText("Check Tracker", CVarGetInteger("gCheckTrackerEnabled", 0)).c_str(), buttonSize)) {
                mCheckTrackerWindow->ToggleVisibility();
            }
        }
        UIWidgets::Spacer(0);
        if (mCheckTrackerSettingsWindow) {
            if (ImGui::Button(GetWindowButtonText("Check Tracker Settings", CVarGetInteger("gCheckTrackerSettingsEnabled", 0)).c_str(), buttonSize)) {
                mCheckTrackerSettingsWindow->ToggleVisibility();
            }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        UIWidgets::PaddedSeparator();

        if (ImGui::BeginMenu("Rando Enhancements"))
        {
            UIWidgets::EnhancementCheckbox("Rando-Relevant Navi Hints", "gRandoRelevantNavi", false, "", UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "Replace Navi's overworld quest hints with rando-related gameplay hints."
            );
            UIWidgets::PaddedEnhancementCheckbox("Random Rupee Names", "gRandomizeRupeeNames", true, false, false, "", UIWidgets::CheckboxGraphics::Cross, true);
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

            UIWidgets::PaddedEnhancementCheckbox("Key Colors Match Dungeon", "gRandoMatchKeyColors", true, false,
                                                  disableKeyColors, disableKeyColorsText, UIWidgets::CheckboxGraphics::Cross, true);
            UIWidgets::Tooltip(
                "Matches the color of small keys and boss keys to the dungeon they belong to. "
                "This helps identify keys from afar and adds a little bit of flair.\n\nThis only "
                "applies to seeds with keys and boss keys shuffled to Any Dungeon, Overworld, or Anywhere.");
            UIWidgets::PaddedEnhancementCheckbox("Quest Item Fanfares", "gRandoQuestItemFanfares", true, false);
            UIWidgets::Tooltip(
                "Play unique fanfares when obtaining quest items "
                "(medallions/stones/songs). Note that these fanfares are longer than usual."
            );
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
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
