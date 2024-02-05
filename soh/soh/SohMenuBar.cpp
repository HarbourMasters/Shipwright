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

    static const char* uiMenuColors[11] = { "White", "Gray", "Indigo", "Red", "Dark Red", "Light Green", "Green", "Dark Green", "Yellow", "Blue", "Purple" };
    typedef enum { WHITE, GRAY, INDIGO, RED, DARK_RED, LIGHT_GREEN, GREEN, DARK_GREEN, YELLOW, BLUE, PURPLE } ColorOption;
    ImVec4 colorChoice = { 1.0f, 1.0f, 1.0f, 1.0f };

    static const char* animatedLinkRotations[4] = { "Disabled", "Rotate Link with D-pad", "Rotate Link with C-buttons", "Rotate Link with Right Stick" };
    static const char* animatedLinkStaticLoops[14] = { "Disabled", "Idle (standing)", "Idle (look around)", "Idle (belt)", "Idle (shield)", "Idle (yawn)",
                                                       "Battle Stance", "Walking (no shield)", "Walking (holding shield)", "Running (no shield)",
                                                       "Running (holding shield)", "Hand on hip", "Spin attack charge", "Look at hand" };
    static const char* animatedLinkRandomizer[4] = { "Disabled", "Random", "Random cycle", "Random cycle (Idle)" };
    static const std::unordered_map<LUS::AudioBackend, const char*> audioBackendsMap = {
        { LUS::AudioBackend::WASAPI, "Windows Audio Session API" },
        { LUS::AudioBackend::PULSE, "PulseAudio" },
        { LUS::AudioBackend::SDL, "SDL" },
    };
    static const char* languageChoice[3] = { "English", "German", "French" };
    static const char* presetOptions[4] = { "Default", "Vanilla Plus", "Enhanced", "Randomizer" };


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
    if (UIWidgets::BeginMenu("Ship", colorChoice)) {
        if (UIWidgets::MenuItem("Hide Menu Bar",
#if !defined(__SWITCH__) && !defined(__WIIU__)
         "F1"
#else
         "[-]"
#endif
        , colorChoice)) {
            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetMenuBar()->ToggleVisibility();
        }
#if !defined(__SWITCH__) && !defined(__WIIU__)
        if (UIWidgets::MenuItem("Toggle Fullscreen", "F11", colorChoice)) {
            LUS::Context::GetInstance()->GetWindow()->ToggleFullscreen();
        }
#endif
        if (UIWidgets::MenuItem("Reset",
#ifdef __APPLE__
                            "Command-R"
#elif !defined(__SWITCH__) && !defined(__WIIU__)
                            "Ctrl+R"
#else
                            ""
#endif
                            , colorChoice)) {
            std::reinterpret_pointer_cast<LUS::ConsoleWindow>(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))->Dispatch("reset");
        }
#if !defined(__SWITCH__) && !defined(__WIIU__)
        if (UIWidgets::MenuItem("Open App Files Folder", "", colorChoice)) {
            std::string filesPath = LUS::Context::GetInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        }
        if (UIWidgets::MenuItem("Quit", "", colorChoice)) {
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
    if (UIWidgets::BeginMenu("Settings", colorChoice))
    {
        if (UIWidgets::BeginMenu("Audio", colorChoice)) {
            UIWidgets::CVarSliderFloat("Master Volume", "gGameMasterVolume", 0.0f, 1.0f, 1.0f, {
                .color = colorChoice,
                .tooltip = "Changes the overall volume of the game.",
                .format = "%.1f %%",
                .isPercentage = true,
            });
            if (UIWidgets::CVarSliderFloat("Main Music Volume", "gMainMusicVolume", 0.0f, 1.0f, 1.0f, {
                .color = colorChoice,
                .tooltip = "Changes the background music volume.",
                .format = "%.1f %%",
                .isPercentage = true,
            })) {
                Audio_SetGameVolume(SEQ_BGM_MAIN, CVarGetFloat("gMainMusicVolume", 1.0f));
            }
            if (UIWidgets::CVarSliderFloat("Sub Music Volume", "gSubMusicVolume", 0.0f, 1.0f, 1.0f, {
                .color = colorChoice,
                .tooltip = "Changes the sub music volume.",
                .format = "%.1f %%",
                .isPercentage = true,
            })) {
                Audio_SetGameVolume(SEQ_BGM_SUB, CVarGetFloat("gSubMusicVolume", 1.0f));
            }
            if (UIWidgets::CVarSliderFloat("Sound Effects Volume", "gSFXMusicVolume", 0.0f, 1.0f, 1.0f, {
                .color = colorChoice,
                .tooltip = "Changes the sub music volume.",
                .format = "%.1f %%",
                .isPercentage = true,
            })) {
                Audio_SetGameVolume(SEQ_SFX, CVarGetFloat("gSFXMusicVolume", 1.0f));
            }
            if (UIWidgets::CVarSliderFloat("Fanfare Volume", "gFanfareVolume", 0.0f, 1.0f, 1.0f, {
                .color = colorChoice,
                .tooltip = "Changes the sub music volume.",
                .format = "%.1f %%",
                .isPercentage = true,
            })) {
                Audio_SetGameVolume(SEQ_FANFARE, CVarGetFloat("gFanfareVolume", 1.0f));
            }

            auto currentAudioBackend = LUS::Context::GetInstance()->GetAudio()->GetAudioBackend();
            if (UIWidgets::Combobox("Audio API (Requires Reload)", &currentAudioBackend, audioBackendsMap, {
                .color = colorChoice,
                .tooltip = "Sets the audio API used by the game. Requires a relaunch to take effect.",
                .disabled = LUS::Context::GetInstance()->GetAudio()->GetAvailableAudioBackends()->size() <= 1,
                .disabledTooltip = "Only one audio API is available on this platform."
            })) {
            LUS::Context::GetInstance()->GetAudio()->SetAudioBackend(currentAudioBackend);
            }
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (UIWidgets::BeginMenu("Controller", colorChoice)) {
            if (mInputEditorWindow) {
                UIWidgets::WindowButton("Controller Mapping", "gControllerConfigurationEnabled", mInputEditorWindow, {
                    .color = colorChoice,
                    .tooltip = "Opens the Controller Configuration window, allowing the mapping of controller inputs.",
                });
            }
            if (mGameControlEditorWindow) {
                UIWidgets::WindowButton("Additional Controller Options", "gGameControlEditorEnabled", mGameControlEditorWindow, {
                    .color = colorChoice,
                    .tooltip = "Opens the Additional Controller Options window, options include D-pad and Ocarina customization.",
                });
            }
            UIWidgets::PaddedSeparator();
        #ifndef __SWITCH__
            UIWidgets::CVarCheckbox("Menubar Controller Navigation", "gControlNav", {
                .color = colorChoice,
                .tooltip = "Allows controller navigation of the SOH menu bar (Settings, Enhancements,...)"
                           "CAUTION: This will disable game inputs while the menubar is visible. D-pad to move between items, "
                           "A to select, and X to grab focus on the menu bar.",
            });
        #endif
            UIWidgets::CVarCheckbox("Show Inputs", "gInputEnabled", {
                .color = colorChoice,
                .tooltip = "Shows currently pressed inputs on the bottom right of the screen.",
            });
            UIWidgets::CVarSliderFloat("Input Scale", "gInputScale", 1.0f, 3.0f, 1.0f, {
                .color = colorChoice,
                .tooltip = "Sets the on screen size of the displayed inputs from the Show Inputs setting.",
                .format = "%.2fx",
            });
            UIWidgets::CVarSliderInt("Simulated Input Lag", "gSimulatedInputLag", 0, 6, 0, {
                .color = colorChoice,
                .tooltip = "Buffers your inputs to be executed a specified amount of frames later.",
                .format = "%d Frames",
            });
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (UIWidgets::BeginMenu("Graphics", colorChoice)) {
            UIWidgets::CVarCombobox("Menu Theme", "gMenuTheme", uiMenuColors, {
                .color = colorChoice,
                .tooltip = "Change the Color Theme of the Menu Bar."
            });
        #ifndef __APPLE__
            const bool disabled_resolutionSlider = CVarGetInteger("gAdvancedResolution.VerticalResolutionToggle", 0) &&
                                                   CVarGetInteger("gAdvancedResolution.Enabled", 0);
            if (UIWidgets::CVarSliderFloat("Internal Resolution", "gInternalResolution", 0.5f, 2.0f, 1.0f, {
                .color = colorChoice,
                .tooltip = "Multiplies your output resolution by the value inputted, as a more intensive but effective form of anti-aliasing.",
                .disabled = disabled_resolutionSlider,
                .format = "%.f %%",
                .isPercentage = true,
            })) {
                LUS::Context::GetInstance()->GetWindow()->SetResolutionMultiplier(CVarGetFloat("gInternalResolution", 1));
            }
        #endif
            
            if (mAdvancedResolutionSettingsWindow) {
                UIWidgets::WindowButton("Advanced Resolution", "gAdvancedResolutionEditorEnabled", mAdvancedResolutionSettingsWindow, {
                    .color = colorChoice,
                    .tooltip = "Opens/Closes the Advanced Resolution Settings Menu.",
                });
            }

        #ifndef __WIIU__
            if (UIWidgets::CVarSliderInt("MSAA", "gMSAAValue", 1, 8, 1, {
                .color = colorChoice,
                .tooltip = "Activates multi-sample anti-aliasing when above 1x up to 8x for 8 samples for every pixel.",
                .format = "%dx",
            })) {
                LUS::Context::GetInstance()->GetWindow()->SetMsaaLevel(CVarGetInteger("gMSAAValue", 1));
            }
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
                static const char* matchingRefreshRateTooltip;
                if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                    matchingRefreshRateTooltip =
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is purely "
                        "visual and does not impact game logic, execution of glitches etc.\n\n"
                        "A higher target FPS than your monitor's refresh rate will waste resources, and might give a worse result.";
                } else {
                    matchingRefreshRateTooltip = 
                        "Uses Matrix Interpolation to create extra frames, resulting in smoother graphics. This is purely "
                        "visual and does not impact game logic, execution of glitches etc.";
                }
                UIWidgets::CVarSliderInt((currentFps == 20) ? "FPS: Original (20)" : "FPS: %d", "gInterpolationFPS", minFps, maxFps, 20, {
                    .color = colorChoice,
                    .tooltip = matchingRefreshRateTooltip,
                    .disabled = matchingRefreshRate,
                    .format = "%d Frames",
                });
            #endif
            } // END FPS Slider

            if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                if (UIWidgets::Button("Match Refresh Rate", {
                    .color = colorChoice,
                    .tooltip = "Matches interpolation value to the current game's window refresh rate."
                })) {
                    int hz = LUS::Context::GetInstance()->GetWindow()->GetCurrentRefreshRate();
                    if (hz >= 20 && hz <= 360) {
                        CVarSetInteger("gInterpolationFPS", hz);
                        LUS::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesOnNextTick();
                    }
                }
            } else {
                UIWidgets::CVarCheckbox("Match Refresh Rate", "gMatchRefreshRate", {
                    .color = colorChoice,
                    .tooltip = "Matches interpolation value to the current game's window refresh rate.",
                });
            }

            if (LUS::Context::GetInstance()->GetWindow()->GetWindowBackend() == LUS::WindowBackend::DX11) {
                UIWidgets::CVarSliderInt(CVarGetInteger("gExtraLatencyThreshold", 80) == 0 ? "Jitter fix: Off" : "Jitter fix: >= %d FPS", "gExtraLatencyThreshold", 0, 360, 80, {
                    .color = colorChoice,
                    .tooltip = "When Interpolation FPS setting is at least this threshold, add one frame of input lag (e.g. 16.6 ms for 60 FPS) in order to avoid jitter."
                               "This setting allows the CPU to work on one frame while GPU works on the previous frame.\n"
                               "This setting should be used when your computer is too slow to do CPU + GPU work in time.",
                    .format = "%d Frames",
                });
            }

            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
            ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, "(Experimental)");
            if (UIWidgets::CVarCombobox("ImGui Menu Scale", "gImGuiScale", imguiScaleOptions, {
                .color = colorChoice,
                .tooltip = "Changes the scaling of the ImGui menu elements",
            })) {
                OTRGlobals::Instance->ScaleImGui();
            }
            UIWidgets::PaddedSeparator(true, true, 3.0f, 3.0f);
            
            static std::unordered_map<LUS::WindowBackend, const char*> windowBackendNames = {
                { LUS::WindowBackend::DX11, "DirectX" },
                { LUS::WindowBackend::SDL_OPENGL, "OpenGL"},
                { LUS::WindowBackend::SDL_METAL, "Metal" },
                { LUS::WindowBackend::GX2, "GX2"}
            };

            LUS::WindowBackend runningWindowBackend = LUS::Context::GetInstance()->GetWindow()->GetWindowBackend();
            LUS::WindowBackend configWindowBackend;
            int configWindowBackendId = LUS::Context::GetInstance()->GetConfig()->GetInt("Window.Backend.Id", -1);
            if (configWindowBackendId != -1 &&
                configWindowBackendId < static_cast<int>(LUS::WindowBackend::BACKEND_COUNT)) {
                configWindowBackend = static_cast<LUS::WindowBackend>(configWindowBackendId);
            } else {
                configWindowBackend = runningWindowBackend;
            }

            if (LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size() <= 1) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }

            auto currentWindowBackend = LUS::Context::GetInstance()->GetWindow()->GetWindowBackend();
            if (UIWidgets::Combobox("Renderer API (Needs reload):", &currentWindowBackend, windowBackendNames, {
                .color = colorChoice,
                .tooltip = "Sets the Renderer API used by the game. Requires a relaunch to take effect.",
                .disabled = LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size() <= 1,
                .disabledTooltip = "Only one Renderer API is available on this platform.",
            })) {
            	LUS::Context::GetInstance()->GetConfig()->SetInt("Window.Backend.Id", static_cast<int>(currentWindowBackend));
            	LUS::Context::GetInstance()->GetConfig()->SetString("Window.Backend.Name", windowBackendNames[currentWindowBackend]);
                CVarSetString("gNextAPI", windowBackendNames[currentWindowBackend]);
            	LUS::Context::GetInstance()->GetConfig()->Save();
            }
            ImGui::Text("Current API: ");
            ImGui::SameLine();
            ImGui::Text(windowBackendNames[LUS::Context::GetInstance()->GetWindow()->GetWindowBackend()]);
            ImGui::Text("Next Reload: ");
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, CVarGetString("gNextAPI", ""));
            
            if (LUS::Context::GetInstance()->GetWindow()->GetAvailableWindowBackends()->size() <= 1) {
                UIWidgets::ReEnableComponent("");
            }
            if (LUS::Context::GetInstance()->GetWindow()->CanDisableVerticalSync()) {
                UIWidgets::CVarCheckbox("Enable Vsync", "gVsyncEnabled", {
                    .color = colorChoice,
                    .tooltip = "Enables Vsync.",
                });
            }
            if (LUS::Context::GetInstance()->GetWindow()->SupportsWindowedFullscreen()) {
                UIWidgets::CVarCheckbox("Windowed fullscreen", "gSdlWindowedFullscreen", {
                    .color = colorChoice,
                    .tooltip = "Enables Fullscreen.",
                });
            }
            if (LUS::Context::GetInstance()->GetWindow()->GetGui()->SupportsViewports()) {
                UIWidgets::CVarCheckbox("Allow multi-windows", "gEnableMultiViewports", {
                    .color = colorChoice,
                    .tooltip = "Allows windows to be able to be dragged off of the main game window. Requires a reload to take effect.",
                });
            }

            // If more filters are added to LUS, make sure to add them to the filters list here
            UIWidgets::CVarCombobox("Texture Filtering (Requires reload)", "gTextureFilter", filters, {
                .color = colorChoice,
                .tooltip = "Changes the filtering of textures.",
                .defaultIndex = FILTER_THREE_POINT,
            });

            LUS::Context::GetInstance()->GetWindow()->GetGui()->GetGameOverlay()->DrawSettings();

            ImGui::EndMenu();
        }

        if (UIWidgets::BeginMenu("Languages", colorChoice)) {
            UIWidgets::CVarCheckbox("Translate Title Screen", "gTitleScreenTranslation", {
                .color = colorChoice,
                .tooltip = "Translates the Title screen into the Language selected below.",
            });
            if (UIWidgets::CVarCombobox("Change Language", "gLanguages", languageChoice, {
                .color = colorChoice,
                .tooltip = "Select an available Language for the game.",
            })) {
                GameInteractor::Instance->ExecuteHooks<GameInteractor::OnSetGameLanguage>();
            }
            ImGui::EndMenu();
        }
        if (UIWidgets::BeginMenu("Accessibility", colorChoice)) {
        #if defined(_WIN32) || defined(__APPLE__)
            UIWidgets::CVarCheckbox("Text to Speech", "gA11yTTS", {
                .color = colorChoice,
                .tooltip = "Enables text to speech for in game dialog.",
            });
        #endif
            UIWidgets::CVarCheckbox("Disable Idle Camera Re-Centering", "gA11yDisableIdleCam", {
                .color = colorChoice,
                .tooltip = "Disables the automatic re-centering of the camera when idle.",
            });
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

extern std::shared_ptr<AudioEditor> mAudioEditorWindow;
extern std::shared_ptr<CosmeticsEditorWindow> mCosmeticsEditorWindow;
extern std::shared_ptr<GameplayStatsWindow> mGameplayStatsWindow;

void DrawEnhancementsMenu() {
    if (UIWidgets::BeginMenu("Enhancements", colorChoice))
    {
        ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));

        
        UIWidgets::CVarCombobox("Preset Selector", "gPreset", presetOptions, {
            .color = colorChoice,
            .tooltip = "Preconfigured Settings Presets:\n"
                       "- Default: Reset all options to their default values.\n"
                       "- Vanilla Plus: Quality of Life features that don't alter gameplay. Recommended for a first playthrough of OoT.\n"
                       "- Enhanced: Quality of life features that might alter gameplay. Recommended for returning players.\n"
                       "- Randomizer: The \"Enhanced\" preset, plus any other enhancements that are recommended for playing Randomizer.",
        });
        DrawPresetSelector(PRESET_TYPE_ENHANCEMENTS);

        UIWidgets::PaddedSeparator();

        if (UIWidgets::BeginMenu("Gameplay", colorChoice))
        {
            if (UIWidgets::BeginMenu("Time Savers", colorChoice))
            {
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

            if (UIWidgets::BeginMenu("Items", colorChoice))
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

            if (UIWidgets::BeginMenu("Difficulty Options", colorChoice))
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
                UIWidgets::CVarCheckbox("Change Red Potion Effect", "gRedPotionEffect", {
                    .color = colorChoice, 
                    .tooltip = "Enable the following changes to the amount of health restored by Red Potions.",
                });
                if (CVarGetInteger("gRedPotionEffect", 0)) {
                    UIWidgets::CVarSliderInt("Red Potion Percentage", "gRedPercentRestore", 1, 100, 100, {
                        .color = colorChoice,
                        .tooltip = "Changes the amount of health restored by Red Potions.",
                        .format = "%d%",
                    });
                    UIWidgets::PaddedSeparator();
                }
                UIWidgets::CVarCheckbox("Change Green Potion Effect", "gGreenPotionEffect", {
                    .color = colorChoice, 
                    .tooltip = "Enable the following changes to the amount of magic restored by Green Potions.",
                });
                if (CVarGetInteger("gGreenPotionEffect", 0)) {
                    UIWidgets::CVarSliderInt("Green Potion Percentage", "gGreenPercentRestore", 1, 100, 100, {
                        .color = colorChoice,
                        .tooltip = "Changes the amount of magic restored by Green Potions.",
                        .format = "%d%",
                    });
                    UIWidgets::PaddedSeparator();
                }
                UIWidgets::CVarCheckbox("Change Blue Potion Effect", "gBluePotionEffect", {
                    .color = colorChoice, 
                    .tooltip = "Enable the following changes to the amount of health and magic restored by Blue Potions.",
                });
                if (CVarGetInteger("gBluePotionEffect", 0)) {
                    UIWidgets::CVarSliderInt("Blue Potion Percentage", "gBluePercentRestore", 1, 100, 100, {
                        .color = colorChoice,
                        .tooltip = "Changes the amount of health and magic restored by Blue Potions.",
                        .format = "%d%",
                    });
                    UIWidgets::PaddedSeparator();
                }
                UIWidgets::CVarCheckbox("Change Milk Effect", "gMilkEffect", {
                    .color = colorChoice, 
                    .tooltip = "Enable the following changes to the amount of health restored by Milk.",
                });
                if (CVarGetInteger("gMilkEffect", 0)) {
                    UIWidgets::CVarSliderInt("Milk Percentage", "gMilkPercentRestore", 1, 100, 100, {
                        .color = colorChoice,
                        .tooltip = "Changes the amount of health restored by Milk.",
                        .format = "%d%",
                    });
                    UIWidgets::PaddedSeparator();
                    UIWidgets::CVarCheckbox("Separate Half Milk Effect", "gSeparateHalfMilkEffect", {
                        .color = colorChoice, 
                        .tooltip = "Enable the following changes to the amount of health restored by Half Milk\n"
                                   "If this is disabled, Half Milk will behave the same as Full Milk.",
                    });
                    if (CVarGetInteger("gSeparateHalfMilkEffect", 0)) {
                        UIWidgets::CVarSliderInt("Half Milk Percentage", "gHalfMilkPercentRestore", 1, 100, 100, {
                            .color = colorChoice,
                            .tooltip = "Changes the amount of health restored by Half Milk.",
                            .format = "%d%",
                        });
                    }
                    UIWidgets::PaddedSeparator();
                }
                UIWidgets::CVarCheckbox("Change Fairy Effect", "gFairyEffect", {
                    .color = colorChoice, 
                    .tooltip = "Enable the following changes to the amount of health restored by Fairies.",
                });
                if (CVarGetInteger("gFairyEffect", 0)) {
                    UIWidgets::CVarSliderInt("Fairy Effect", "gFairyPercentRestore", 1, 100, 100, {
                        .color = colorChoice,
                        .tooltip = "Changes the amount of health restored by Fairies.",
                        .format = "%d%",
                    });
                    UIWidgets::PaddedSeparator();
                }
                UIWidgets::CVarCheckbox("Change Fairy Revive Effect", "gFairyReviveEffect", {
                    .color = colorChoice, 
                    .tooltip = "Changes the amount of health restored by Fairy Revivals.",
                });
                if (CVarGetInteger("gFairyReviveEffect", 0)) {
                    UIWidgets::CVarSliderInt("Fairy Revive Effect", "gFairyRevivePercentRestore", 1, 100, 100, {
                        .color = colorChoice,
                        .tooltip = "Changes the amount of health restored by Fairy Revivals.",
                        .format = "%d%",
                    });
                }

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

            UIWidgets::Spacer(0);

            if (UIWidgets::BeginMenu("Reduced Clutter", colorChoice))
            {
                ImGui::BeginTable("Reduced Clutter", 1);
                ImGui::TableNextColumn();

                UIWidgets::CVarCheckbox("Mute Low HP Alarm", "gLowHpAlarm", {
                    .color = colorChoice, 
                    .tooltip = "Disable the low health beeping sound.",
                });
                UIWidgets::CVarCheckbox("Minimal UI", "gMinimalUI", {
                    .color = colorChoice, 
                    .tooltip = "Hides most of the UI when not needed\nNote: Doesn't activate until after loading a new scene.",
                });
                UIWidgets::CVarCheckbox("Disable Navi Call Audio", "gDisableNaviCallAudio", {
                    .color = colorChoice, 
                    .tooltip = "Disables the voice audio when Navi calls you.",
                });
                UIWidgets::CVarCheckbox("Disable Hot/Underwater Warning Text", "gDisableTunicWarningText", {
                    .color = colorChoice, 
                    .tooltip = "Disables warning text when you don't have on the Goron/Zora Tunic in Hot/Underwater conditions.",
                });
                ImGui::EndTable();
                ImGui::EndMenu();
            }

            UIWidgets::Spacer(0);

            UIWidgets::CVarCheckbox("Visual Stone of Agony", "gVisualAgony", {
                .color = colorChoice, 
                .tooltip = "Displays an icon and plays a sound when Stone of Agony should be activated, for those without rumble.",
            });
            UIWidgets::CVarCheckbox("Assignable Tunics and Boots", "gAssignableTunicsAndBoots", {
                .color = colorChoice, 
                .tooltip = "Allows equipping the tunic and boots to c-buttons.",
            });
            UIWidgets::CVarCheckbox("Equipment Toggle", "gEquipmentCanBeRemoved", {
                .color = colorChoice, 
                .tooltip = "Allows equipment to be removed by toggling it off on the equipment subscreen.",
            });
            UIWidgets::CVarCheckbox("Link's Cow in Both Time Periods", "gCowOfTime", {
                .color = colorChoice, 
                .tooltip = "Allows the Lon Lon Ranch obstacle course reward to be shared across time periods.",
            });
            UIWidgets::CVarCheckbox("Enable visible guard vision", "gGuardVision", {
                .color = colorChoice, 
                .tooltip = "Displays a visual aid indicating where the guards are looking.",
            });
            UIWidgets::CVarCheckbox("Enable passage of time on file select", "gTimeFlowFileSelect", {
                .color = colorChoice, 
                .tooltip = "Enables the Day/Night cycle while idling on the File Select screen.",
            });
            UIWidgets::CVarCheckbox("Item counts in messages", "gInjectItemCounts", {
                .color = colorChoice, 
                .tooltip = "Injects item counts in pickup messages, like golden skulltula tokens and heart pieces.",
            });
            UIWidgets::CVarCheckbox("Pull grave during the day", "gDayGravePull", {
                .color = colorChoice, 
                .tooltip = "Allows graves to be pulled when child during the day.",
            });
            UIWidgets::CVarCheckbox("Dogs follow you everywhere", "gDogFollowsEverywhere", {
                .color = colorChoice, 
                .tooltip = "Allows dogs to follow you anywhere you go, even if you leave the market.",
            });
            UIWidgets::CVarCheckbox("Don't require input for Credits sequence", "gNoInputForCredits", {
                .color = colorChoice, 
                .tooltip = "Removes the input requirement on textboxes after defeating Ganon, allowing Credits sequence to continue to progress.",
            });

            bool disabledBlueFireArrows =
                IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BLUE_FIRE_ARROWS);
            static const char* disabledBlueFireArrowsText = "This option is enabled because \"Blue Fire Arrows\" is turned on in a loaded savefile.";
            UIWidgets::CVarCheckbox("Blue Fire Arrows", "gBlueFireArrows", {
                .color = colorChoice, 
                .tooltip = "Allows Ice Arrows to melt red ice. May require a room reload if toggled during gameplay.",
                .disabled = disabledBlueFireArrows,
                .disabledTooltip = disabledBlueFireArrowsText,
            });

            bool disabledSunLightArrows =
                IS_RANDO && OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SUNLIGHT_ARROWS);
            static const char* disabledSunLightArrowsText = "This option is enabled because \"Sunlight Arrows\" is turned on in a loaded savefile.";
            UIWidgets::CVarCheckbox("Sunlight Arrows", "gSunlightArrows", {
                .color = colorChoice, 
                .tooltip = "Allows Light Arrows to activate sun switches. May require a room reload if toggled during gameplay.",
                .disabled = disabledSunLightArrows,
                .disabledTooltip = disabledSunLightArrowsText,
            });

            UIWidgets::CVarCheckbox("Disable Crit wiggle", "gDisableCritWiggle", {
                .color = colorChoice, 
                .tooltip = "Disable random camera wiggle at low health.",
            });
            UIWidgets::CVarCheckbox("Enemy Health Bars", "gEnemyHealthBar", {
                .color = colorChoice, 
                .tooltip = "Renders a health bar for enemies when Z-Targeted.",
            });
            UIWidgets::CVarCheckbox("Targetable Hookshot Reticle", "gHookshotableReticle", {
                .color = colorChoice, 
                .tooltip = "Use a different color when aiming at hookshotable collision.",
            });

            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (UIWidgets::BeginMenu("Graphics", colorChoice))
        {
            ImGui::BeginTable("Graphics", 2);
            ImGui::TableNextColumn();
            UIWidgets::Spacer(0);
            ImGui::Text("Mod Options");
            UIWidgets::PaddedSeparator(true, true, 2.0f, 0.0f);

            if (UIWidgets::CVarCheckbox("Use Alternate Assets", "gAltAssets", {
                .color = colorChoice, 
                .tooltip = "Toggle between standard assets and alternate assets. Usually mods will indicate if this setting has to be used or not.",
            })); {
                CVarSetInteger("gAltAssets", !CVarGetInteger("gAltAssets", 0));
                ToggleAltAssetsAtEndOfFrame = true;
            }

            UIWidgets::CVarCheckbox("Disable Bomb Billboarding", "gDisableBombBillboarding", {
                .color = colorChoice, 
                .tooltip = "Disables bombs always rotating to face the camera. To be used in conjunction with mods that want to replace bombs with 3D objects.",
            });
            UIWidgets::CVarCheckbox("Disable Grotto Fixed Rotation", "gDisableGrottoRotation", {
                .color = colorChoice, 
                .tooltip = "Disables grottos rotating with the camera. To be used in conjunction with mods that want to replace grottos with 3D objects.",
            });
            UIWidgets::CVarSliderInt("Text Spacing", "gTextSpacing", 4, 6, 6, {
                .color = colorChoice,
                .tooltip = "Space between text characters (useful for HD font textures).",
                .format = "%d",
            });

            UIWidgets::PaddedSeparator();

            UIWidgets::CVarCheckbox("Disable LOD", "gDisableLOD", {
                .color = colorChoice, 
                .tooltip = "Turns off the Level of Detail setting, making models use their higher-poly variants at any distance.",
            });
            UIWidgets::CVarCheckbox("Disable Draw Distance", "gDisableDrawDistance", {
                .color = colorChoice, 
                .tooltip = "Turns off the objects draw distance, making objects being visible from a longer range.",
            });
            if (CVarGetInteger("gDisableDrawDistance", 0) == 0) {
                CVarSetInteger("gDisableKokiriDrawDistance", 0);
            } else {
                UIWidgets::CVarCheckbox("Kokiri Draw Distance", "gDisableKokiriDrawDistance", {
                .color = colorChoice, 
                .tooltip = "The Kokiri are mystical beings that fade into view when approached\nEnabling this will remove their draw distance.",
                });
            }

            UIWidgets::CVarCheckbox("N64 Mode", "gLowResMode", {
                .color = colorChoice, 
                .tooltip = "Sets aspect ratio to 4:3 and lowers resolution to 240p, the N64's native resolution.",
            });
            UIWidgets::CVarCheckbox("Enable 3D Dropped items/projectiles", "gNewDrops", {
                .color = colorChoice, 
                .tooltip = "Change most 2D items and projectiles on the overworld to their 3D versions.",
            });
            UIWidgets::CVarCheckbox("Disable Black Bar Letterboxes", "gDisableBlackBars", {
                .color = colorChoice, 
                .tooltip = "Disables Black Bar Letterboxes during cutscenes and Z-targeting\n"
                           "Note: there may be minor visual glitches that were covered up by the black bars.\n"
                           "Please disable this setting before reporting a bug.",
            });
            UIWidgets::CVarCheckbox("Dynamic Wallet Icon", "gDynamicWalletIcon", {
                .color = colorChoice, 
                .tooltip = "Changes the rupee in the wallet icon to match the wallet size you currently have.",
            });
            UIWidgets::CVarCheckbox("Always show dungeon entrances", "gAlwaysShowDungeonMinimapIcon", {
                .color = colorChoice, 
                .tooltip = "Always shows dungeon entrance icons on the minimap.",
            });
            UIWidgets::CVarCheckbox("Show Gauntlets in First Person", "gFPSGauntlets", {
                .color = colorChoice, 
                .tooltip = "Renders Gauntlets when using the Bow and Hookshot like in OOT3D.",
            });
            UIWidgets::CVarCombobox("Fix Vanishing Paths", "gSceneSpecificDirtPathFix", zFightingOptions, {
                .color = colorChoice,
                .tooltip = "Disabled: Paths vanish more the higher the resolution (Z-fighting is based on resolution)\n"
                           "Consistent: Certain paths vanish the same way in all resolutions\n"
                           "No Vanish: Paths do not vanish, Link seems to sink in to some paths\n"
                           "This might affect other decal effects.",
            });
            if (CVarGetInteger("gSceneSpecificDirtPathFix", ZFIGHT_FIX_DISABLED) && gPlayState != NULL) {
                UpdateDirtPathFixState(gPlayState->sceneNum);
            }
            UIWidgets::CVarCheckbox("More info in file select", "gFileSelectMoreInfo", {
                .color = colorChoice, 
                .tooltip = "Shows what items you have collected in the file select screen.",
            });

            ImGui::TableNextColumn();
            ImGui::Text("Animated Link in Pause Menu          ");
            UIWidgets::PaddedSeparator();
            UIWidgets::CVarCombobox("Rotation", "gPauseLiveLinkRotation", animatedLinkRotations, {
                .color = colorChoice,
                .tooltip = "Change how Link rotates in the Pause Menu.",
            });
            if (CVarGetInteger("gPauseLiveLinkRotation", 0) != 0) {
                UIWidgets::CVarSliderInt("Rotation Speed", "gPauseLiveLinkRotationSpeed", 1, 20, 1, {
                    .color = colorChoice,
                    .tooltip = "The speed at which Link rotates.",
                    .format = "%d",
                });
            }
            bool disabledStaticLoop = CVarGetInteger("gRandomLiveLink", 0);
            static const char* disabledStaticLoopText = "This option is disabled because \"Randomize\" is not set to \"Disabled\".";
            UIWidgets::CVarCombobox("Static loop", "gPauseLiveLink", animatedLinkStaticLoops, {
                .color = colorChoice,
                .tooltip = "Change Link's pose in the Pause Menu.",
                .disabled = disabledStaticLoop,
                .disabledTooltip = disabledStaticLoopText,
            });
            UIWidgets::CVarCombobox("Randomize", "gRandomLiveLink", animatedLinkRandomizer, {
                .color = colorChoice,
                .tooltip = "Randomize Link's pose:\n"
                           "- Disable: No random animations.\n"
                           "- Random: Randomize the animation played each time you open the menu.\n"
                           "- Random cycle: Randomize the animation played on the menu after a certain time.\n"
                           "- Random cycle (Idle): Randomize the animation played on the menu after a certain time (Idle animations only).",
            });
            if (CVarGetInteger("gRandomLiveLink", 0) != 0) {
                uint32_t liveLink = (CVarGetInteger("gRandomLiveLink", 0) + 14);
                CVarSetInteger("gPauseLiveLink", liveLink);
            }
            if (CVarGetInteger("gRandomLiveLink", 0) >= 2) {
                UIWidgets::CVarSliderInt("Frame to wait", "gMinFrameCount", 1, 1000, 0, {
                    .color = colorChoice,
                    .tooltip = "The time before Link's pose changes.",
                    .format = "%d",
                });
            }
            UIWidgets::PaddedSeparator();

            ImGui::EndTable();
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (UIWidgets::BeginMenu("Fixes", colorChoice))
        {
            ImGui::BeginTable("Fixes", 2);
            ImGui::TableNextColumn();

            UIWidgets::CVarCheckbox("Fix L&R Pause menu", "gUniformLR", {
                .color = colorChoice, 
                .tooltip = "Makes the L and R buttons in the pause menu the same color.",
            });
            UIWidgets::CVarCheckbox("Fix L&Z Page switch in Pause menu", "gNGCKaleidoSwitcher", {
                .color = colorChoice, 
                .tooltip = "Makes L and R switch pages like on the GameCube, Z opens the Debug Menu instead.",
            });
            UIWidgets::CVarCheckbox("Fix Dungeon entrances", "gFixDungeonMinimapIcon", {
                .color = colorChoice, 
                .tooltip = "Removes the dungeon entrance icon on the top-left corner of the screen when no dungeon is present on the current map.",
            });
            UIWidgets::CVarCheckbox("Fix Two Handed idle animations", "gTwoHandedIdle", {
                .color = colorChoice, 
                .tooltip = "Re-enables the two-handed idle animation, a seemingly finished animation that was disabled on accident in the original game.",
            });
            UIWidgets::CVarCheckbox("Fix the Gravedigging Tour Glitch", "gGravediggingTourFix", {
                .color = colorChoice, 
                .tooltip = "RemFixes a bug where the Gravedigging Tour Heart Piece disappears if the area reloads.",
            });
            UIWidgets::CVarCheckbox("Fix Deku Nut upgrade", "gDekuNutUpgradeFix", {
                .color = colorChoice, 
                .tooltip = "Prevents the Forest Stage Deku Nut upgrade from becoming unobtainable after receiving the Poacher's Saw.",
            });
            UIWidgets::CVarCheckbox("Fix Navi text HUD position", "gNaviTextFix", {
                .color = colorChoice, 
                .tooltip = "Correctly centers the Navi text prompt on the HUD's C-Up button.",
            });
            UIWidgets::CVarCheckbox("Fix Anubis fireballs", "gAnubisFix", {
                .color = colorChoice, 
                .tooltip = "Correctly centers the Navi text prompt on the HUD's C-Up button.",
            });
            UIWidgets::CVarCheckbox("Fix Megaton Hammer crouch stab", "gCrouchStabHammerFix", {
                .color = colorChoice, 
                .tooltip = "Make the Megaton Hammer's crouch stab able to destroy rocks without first swinging it normally.",
            });
            if (!CVarGetInteger("gCrouchStabHammerFix", 0)) {
                CVarClear("gCrouchStabFix");
            } else {
                UIWidgets::Spacer(0);
                ImGui::SameLine(0, 15.0f);
                UIWidgets::CVarCheckbox("Remove power crouch stab", "gCrouchStabFix", {
                    .color = colorChoice, 
                    .tooltip = "Make crouch stabbing always do the same damage as a regular slash.",
                });
                UIWidgets::PaddedSeparator();
            }
            UIWidgets::CVarCheckbox("Fix credits timing", "gCreditsFix", {
                .color = colorChoice, 
                .tooltip = "Extend certain credits scenes so the music lines up properly with the visuals.",
            });
            UIWidgets::CVarCheckbox("Fix Gerudo Warrior's clothing colors", "gGerudoWarriorClothingFix", {
                .color = colorChoice, 
                .tooltip = "Prevent the Gerudo Warrior's clothes changing color when changing Link's tunic or using bombs in front of her.",
            });
            UIWidgets::CVarCheckbox("Fix Camera Drift", "gFixCameraDrift", {
                .color = colorChoice, 
                .tooltip = "Fixes camera slightly drifting to the left when standing still due to a math error.",
            });
            UIWidgets::CVarCheckbox("Fix Camera Swing", "gFixCameraSwing", {
                .color = colorChoice, 
                .tooltip = "Fixes camera getting stuck on collision when standing still, also fixes slight shift back in camera when stop moving.",
            });
            UIWidgets::CVarCheckbox("Fix Hanging Ledge Swing Rate", "gFixHangingLedgeSwingRate", {
                .color = colorChoice, 
                .tooltip = "Fixes camera swing rate when player falls off a ledge and camera swings around.",
            });
            UIWidgets::CVarCheckbox("Fix Missing Jingle after 5 Silver Rupees", "gSilverRupeeJingleExtend", {
                .color = colorChoice, 
                .tooltip = "Adds 5 higher pitches for the Silver Rupee Jingle for the rooms with more than 5 Silver Rupees.",
            });
            if (UIWidgets::CVarCheckbox("Fix out of bounds textures", "gFixTexturesOOB", {
                .color = colorChoice, 
                .tooltip = "Fixes authentic out of bounds texture reads, instead loading textures with the correct size.",
            })); {
                ApplyAuthenticGfxPatches();
            }
            bool disabledFixPoacherSaw = CVarGetInteger("gSkipText", 0);
            static const char* disabledFixPoacherSawText = "This option is enabled because \"Skip Text\" is enabled.";
            UIWidgets::CVarCheckbox("Fix Poacher's Saw Softlock", "gFixSawSoftlock", {
                .color = colorChoice, 
                .tooltip = "Prevents the Poacher's Saw softlock from mashing through the text, or with Skip Text enabled.",
                .disabled = disabledFixPoacherSaw,
                .disabledTooltip = disabledFixPoacherSawText,
            });
            UIWidgets::CVarCheckbox("Fix Bush Item Drops", "gBushDropFix", {
                .color = colorChoice, 
                .tooltip = "Fixes the bushes to drop items correctly rather than spawning undefined items.",
            });
            
            ImGui::EndTable();
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (UIWidgets::BeginMenu("Restoration", colorChoice))
        {
            ImGui::BeginTable("Restoration", 2);
            ImGui::TableNextColumn();
            UIWidgets::CVarCheckbox("Authentic Logo Screen", "gAuthenticLogo", {
                .color = colorChoice, 
                .tooltip = "Hide the game version and build details and display the authentic model and texture on the boot logo start screen.",
            });
            UIWidgets::CVarCheckbox("Red Ganon blood", "gRedGanonBlood", {
                .color = colorChoice, 
                .tooltip = "Restore the original red blood from NTSC 1.0/1.1. Disable for green blood.",
            });
            UIWidgets::CVarCheckbox("Fish while hovering", "gHoverFishing", {
                .color = colorChoice, 
                .tooltip = "Restore a bug from NTSC 1.0 that allows casting the Fishing Rod while using the Hover Boots.",
            });
            UIWidgets::CVarCheckbox("N64 Weird Frames", "gN64WeirdFrames", {
                .color = colorChoice, 
                .tooltip = "Restores N64 Weird Frames allowing weirdshots to behave the same as N64.",
            });
            UIWidgets::CVarCheckbox("Bombchus out of bounds", "gBombchusOOB", {
                .color = colorChoice, 
                .tooltip = "Allows bombchus to explode out of bounds, similar to GameCube and Wii VC.",
            });
            UIWidgets::CVarCheckbox("Quick Putaway", "gQuickPutaway", {
                .color = colorChoice, 
                .tooltip = "Restore a bug from NTSC 1.0 that allows putting away an item without an animation and performing Putaway Ocarina Items.",
            });
            UIWidgets::CVarCheckbox("Restore old Gold Skulltula cutscene", "gGsCutscene", {
                .color = colorChoice, 
                .tooltip = "Enables an unused cutscene for Gold Skulltulas.",
            });
            UIWidgets::CVarCheckbox("Quick Bongo Kill", "gQuickBongoKill", {
                .color = colorChoice, 
                .tooltip = "Restore a bug from NTSC 1.0 that allows bypassing Bongo Bongo's intro cutscene to quickly kill him.",
            });
            ImGui::EndTable();
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (UIWidgets::BeginMenu("Extra Modes", colorChoice)) {
            ImGui::BeginTable("Extra Modes", 2);
            ImGui::TableNextColumn();
            if (UIWidgets::CVarCombobox("Mirrored World", "gMirroredWorldMode", mirroredWorldModes, {
                .color = colorChoice,
                .tooltip = "Mirrors the world horizontally\n\n"
                           "- Always: Always mirror the world.\n"
                           "- Random: Randomly decide to mirror the world on each scene change.\n"
                           "- Random (Seeded): Scenes are mirrored based on the current randomizer seed/file.\n"
                           "- Dungeons: Mirror the world in Dungeons.\n"
                           "- Dungeons (Vanilla): Mirror the world in vanilla Dungeons.\n"
                           "- Dungeons (MQ): Mirror the world in MQ Dungeons.\n"
                           "- Dungeons Random: Randomly decide to mirror the world in Dungeons.\n"
                           "- Dungeons Random (Seeded): Dungeons are mirrored based on the current randomizer seed/file.",
                .defaultIndex = MIRRORED_WORLD_OFF,
            })) {
                if (gPlayState != NULL) {
                    UpdateMirrorModeState(gPlayState->sceneNum);
                }
            }
            UIWidgets::CVarCombobox("Enemy Randomizer", "gRandomizedEnemies", enemyRandomizerModes, {
                .color = colorChoice,
                .tooltip = "Replaces fixed enemies throughout the game with a random enemy. Bosses, mini-bosses and a few specific regular enemies are excluded.\n"
                           "Enemies that need more than Deku Nuts + either Deku Sticks or a sword to kill are excluded from spawning in \"clear enemy\" rooms.\n"
                           "- Random: Enemies are randomized every time you load a room.\n"
                           "- Random (Seeded): Enemies are randomized based on the current randomizer seed/file.",
                .defaultIndex = ENEMY_RANDOMIZER_OFF,
            });
            UIWidgets::CVarCheckbox("Ivan the Fairy (Coop Mode)", "gIvanCoopModeEnabled", {
                .color = colorChoice, 
                .tooltip = "Enables Ivan the Fairy upon the next map change. Player 2 can control Ivan and \n"
                           "press the C-Buttons to use items and mess with Player 1!.",
            });
            UIWidgets::CVarCheckbox("Rupee Dash Mode", "gRupeeDash", {
                .color = colorChoice, 
                .tooltip = "Rupees reduced over time, Link suffers damage when the count hits 0.",
            });
            if (CVarGetInteger("gRupeeDash", 0)) {
                UIWidgets::CVarSliderInt("Rupee Dash Interval", "gDashInterval", 3, 10, 5, {
                    .color = colorChoice,
                    .tooltip = "Interval between Rupee reduction in Rupee Dash Mode.",
                    .format = "%d Seconds",
                });
            }
            UIWidgets::CVarCheckbox("Shadow Tag Mode", "gShadowTag", {
                .color = colorChoice, 
                .tooltip = "A wallmaster follows Link everywhere, don't get caught!",
            });
            
            ImGui::TableNextColumn();
            UIWidgets::CVarCheckbox("Additional Traps             ", "gAddTraps.enabled", {
                .color = colorChoice, 
                .tooltip = "Enables additional Trap variants.",
            });
            if (CVarGetInteger("gAddTraps.enabled", 0)) {
                ImGui::Text("Tier 1 Traps:");
                UIWidgets::CVarCheckbox("Freeze Traps", "gAddTraps.Ice", {
                    .color = colorChoice, 
                    .tooltip = "Standard Freeze Trap.",
                });
                UIWidgets::CVarCheckbox("Burn Traps", "gAddTraps.Burn", {
                    .color = colorChoice, 
                    .tooltip = "Standard Burn Trap, better watch that Shield!",
                });
                UIWidgets::CVarCheckbox("Shock Traps", "gAddTraps.Shock", {
                    .color = colorChoice, 
                    .tooltip = "Standard Shock Trap, metal weapons will unequi... oh, wrong game.",
                });
                UIWidgets::PaddedSeparator();
                ImGui::Text("Tier 2 Traps:");
                UIWidgets::CVarCheckbox("Knockback Traps", "gAddTraps.Knock", {
                    .color = colorChoice, 
                    .tooltip = "Weak Knockback Trap.",
                });
                UIWidgets::CVarCheckbox("Speed Traps", "gAddTraps.Speed", {
                    .color = colorChoice, 
                    .tooltip = "Briefly pretend that running isn't a thing yet.",
                });
                UIWidgets::CVarCheckbox("Bomb Traps", "gAddTraps.Bomb", {
                    .color = colorChoice, 
                    .tooltip = "Instantly explode, might be helpful, might not be.",
                });
                UIWidgets::PaddedSeparator();
                ImGui::Text("Tier 3 Traps:");
                UIWidgets::CVarCheckbox("Void Traps", "gAddTraps.Void", {
                    .color = colorChoice, 
                    .tooltip = "Void out like you missed that Loading Zone!",
                });
                UIWidgets::CVarCheckbox("Ammo Traps", "gAddTraps.Ammo", {
                    .color = colorChoice, 
                    .tooltip = "Halves all Ammo, being over-encumbered is so 2002.",
                });
                UIWidgets::CVarCheckbox("Death Traps", "gAddTraps.Kill", {
                    .color = colorChoice, 
                    .tooltip = "Death awaits you.",
                });
                UIWidgets::CVarCheckbox("Teleport Traps", "gAddTraps.Tele", {
                    .color = colorChoice, 
                    .tooltip = "All around the world, that's where you'll be.",
                });
            }

            ImGui::EndTable();
            ImGui::EndMenu();
        }

        UIWidgets::PaddedSeparator(false, true);
        // Autosave enum value of 1 is the default in presets and the old checkbox "on" state for backwards compatibility
        UIWidgets::CVarCombobox("Autosave", "gAutosave", autosaveLabels, {
                .color = colorChoice,
                .tooltip = "Automatically save the game when changing locations and/or obtaining items.\n"
                           "Major items exclude rupees and health/magic/ammo refills (but include bombchus unless bombchu drops are enabled)",
                .defaultIndex = AUTOSAVE_OFF,
        });
        UIWidgets::PaddedSeparator(true, true, 2.0f, 2.0f);
        ImGui::EndDisabled();

        if (mCosmeticsEditorWindow) {
            UIWidgets::WindowButton("Cosmetics Editor", "gCosmeticsEditorEnabled", mCosmeticsEditorWindow, {
                .color = colorChoice,
                .tooltip = "Customize the games cosmetics.",
            });
        }

        if (mAudioEditorWindow) {
            UIWidgets::WindowButton("Audio Editor", "gAudioEditor.WindowOpen", mAudioEditorWindow, {
                .color = colorChoice,
                .tooltip = "Customize the games audio.",
            });
        }

        if (mGameplayStatsWindow) {
            UIWidgets::WindowButton("Gameplay Stats", "gGameplayStatsEnabled", mGameplayStatsWindow, {
                .color = colorChoice,
                .tooltip = "Check the statistics of your current File.",
            });
        }
        

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));

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
    if (UIWidgets::BeginMenu("Cheats", colorChoice))
    {
        ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8.0f);
        ImGui::BeginTable("Cheats", 2);
        ImGui::TableNextColumn();

        ImGui::Text("Inventory:");
        UIWidgets::CVarCheckbox("Super Tunic", "gSuperTunic", {
            .color = colorChoice, 
            .tooltip = "Makes every tunic have the effects of every other tunic.",
        });
        UIWidgets::CVarCheckbox("Easy ISG", "gEzISG", {
            .color = colorChoice, 
            .tooltip = "Passive Infinite Sword Glitch, it makes your sword's swing effect and hitbox stay active indefinitely.",
        });
        UIWidgets::CVarCheckbox("Easy QPA", "gEzQPA", {
            .color = colorChoice, 
            .tooltip = "Gives you the glitched damage value of the quick put away glitch.",
        });
        UIWidgets::CVarCheckbox("Timeless Equipment", "gTimelessEquipment", {
            .color = colorChoice, 
            .tooltip = "Allows any item to be equipped, regardless of age Also allows Child to use Adult strength upgrades.",
        });
        UIWidgets::CVarCheckbox("Unrestricted Items", "gNoRestrictItems", {
            .color = colorChoice, 
            .tooltip = "Allows you to use any item at any location.",
        });
        UIWidgets::CVarCheckbox("Fireproof Deku Shield", "gFireproofDekuShield", {
            .color = colorChoice, 
            .tooltip = "Prevents the Deku Shield from burning on contact with fire.",
        });
        UIWidgets::CVarCheckbox("Shield with Two-Handed Weapons", "gShieldTwoHanded", {
            .color = colorChoice, 
            .tooltip = "This allows you to put up your shield with any two-handed weapon in hand except for Deku Sticks.",
        });
        UIWidgets::CVarCombobox("Deku Sticks", "gDekuStickCheat", DekuStickCheat, {
            .color = colorChoice,
            .tooltip = "Modify the Deku Sticks behavior.",
            .defaultIndex = DEKU_STICK_NORMAL,
        });
        UIWidgets::CVarSliderFloat("Bomb Timer Multiplier", "gBombTimerMultiplier", 0.5f, 5.0f, 1.0f, {
            .color = colorChoice,
            .tooltip = "Change the fuse timer on Bombs.",
            .format = "%.2fx",
        });
        UIWidgets::CVarCheckbox("Hookshot Everything", "gHookshotEverything", {
            .color = colorChoice, 
            .tooltip = "Makes every surface in the game hookshot-able.",
        });
        UIWidgets::CVarSliderFloat("Hookshot Reach Multiplier", "gCheatHookshotReachMultiplier", 1.0f, 5.0f, 1.0f, {
            .color = colorChoice,
            .tooltip = "Change the Hookshots reach.",
            .format = "%.2fx",
        });
        if (UIWidgets::Button("Change Age", {
            .color = colorChoice,
            .tooltip = "Switches Link's age and reloads the area.",
        })) {
            CVarSetInteger("gSwitchAge", 1);
        }
        if (UIWidgets::Button("Clear Cutscene Pointer", {
            .color = colorChoice,
            .tooltip = "Clears the cutscene pointer to a value safe for wrong warps.",
        })) {
            GameInteractor::RawAction::ClearCutscenePointer();
        }
        ImGui::TableNextColumn();
        UIWidgets::Spacer(0);

        if (UIWidgets::BeginMenu("Infinite...", colorChoice)) {
            UIWidgets::CVarCheckbox("Money", "gInfiniteMoney", {
                .color = colorChoice, 
                .tooltip = "Infinite Rupees.",
            });
            UIWidgets::CVarCheckbox("Health", "gInfiniteHealth", {
                .color = colorChoice, 
                .tooltip = "Infinite Health.",
            });
            UIWidgets::CVarCheckbox("Ammo", "gInfiniteAmmo", {
                .color = colorChoice, 
                .tooltip = "Infinite Ammo.",
            });
            UIWidgets::CVarCheckbox("Magic", "gInfiniteMagic", {
                .color = colorChoice, 
                .tooltip = "Infinite Magic.",
            });
            UIWidgets::CVarCheckbox("Nayru's Love", "gInfiniteNayru", {
                .color = colorChoice, 
                .tooltip = "Infinite Nayru's Love Effect.",
            });
            UIWidgets::CVarCheckbox("Epona Boost", "gInfiniteEpona", {
                .color = colorChoice, 
                .tooltip = "Infinite Carrots for Epona.",
            });
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(0);

        if (UIWidgets::BeginMenu("Save States", colorChoice)) {
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
            UIWidgets::CVarCheckbox("I promise I have read the warning", "gSaveStatePromise", {
                .color = colorChoice, 
                .tooltip = "I promise I have read the warning.",
            });
            if (CVarGetInteger("gSaveStatePromise", 0)) {
                UIWidgets::CVarCheckbox("I understand, enable save states", "gSaveStatesEnabled", {
                    .color = colorChoice, 
                    .tooltip = "F5 to save, F6 to change slots, F7 to load.",
                });
            }
            ImGui::EndMenu();
        }

        UIWidgets::Spacer(2.0f);
        ImGui::Text("Behavior:");
        UIWidgets::PaddedSeparator();
        UIWidgets::CVarCheckbox("No Clip", "gNoClip", {
            .color = colorChoice, 
            .tooltip = "Allows you to walk through walls.",
        });
        UIWidgets::CVarCheckbox("Climb Everything", "gClimbEverything", {
            .color = colorChoice, 
            .tooltip = "Makes every surface in the game climbable.",
        });
        UIWidgets::CVarCheckbox("Moon Jump on L", "gMoonJumpOnL", {
            .color = colorChoice, 
            .tooltip = "Holding L makes you float into the air.",
        });
        UIWidgets::CVarCheckbox("Easy Frame Advancing", "gCheatEasyPauseBufferEnabled", {
            .color = colorChoice, 
            .tooltip = "Continue holding START button when unpausing to only advance a single frame and then re-pause.",
        });
        bool forceInputBuffering = CVarGetInteger("gCheatEasyPauseBufferEnabled", 0);
        static const char* forceInputBufferingText = "This setting is forcefully enabled because \"Easy Frame Advancing\" is enabled";
        UIWidgets::CVarCheckbox("Easy Input Buffering", "gCheatEasyInputBufferingEnabled", {
            .color = colorChoice, 
            .tooltip = "Inputs that are held down while the Subscreen is closing will be pressed when the game is resumed.",
            .disabled = forceInputBuffering,
            .disabledTooltip = forceInputBufferingText,
        });
        UIWidgets::CVarCheckbox("Drops Don't Despawn", "gDropsDontDie", {
            .color = colorChoice, 
            .tooltip = "Drops from enemies, grass, etc. don't disappear after a set amount of time.",
        });
        UIWidgets::CVarCheckbox("Fish Don't despawn", "gNoFishDespawn", {
            .color = colorChoice, 
            .tooltip = "Prevents fish from automatically despawning after a while when dropped.",
        });
        UIWidgets::CVarCheckbox("Bugs Don't despawn", "gNoBugsDespawn", {
            .color = colorChoice, 
            .tooltip = "Prevents bugs from automatically despawning after a while when dropped.",
        });
        UIWidgets::CVarCheckbox("Freeze Time", "gFreezeTime", {
            .color = colorChoice, 
            .tooltip = "Freezes the time of day.",
        });
        UIWidgets::CVarCheckbox("Time Sync", "gTimeSync", {
            .color = colorChoice, 
            .tooltip = "This syncs the ingame time with the real world time.",
        });
        UIWidgets::CVarCheckbox("No ReDead/Gibdo Freeze", "gNoRedeadFreeze", {
            .color = colorChoice, 
            .tooltip = "Prevents ReDeads and Gibdos from being able to freeze you with their scream.",
        });

        {
            static int32_t betaQuestEnabled = CVarGetInteger("gEnableBetaQuest", 0);
            static int32_t lastBetaQuestEnabled = betaQuestEnabled;
            static int32_t betaQuestWorld = CVarGetInteger("gBetaQuestWorld", 0xFFEF);
            static int32_t lastBetaQuestWorld = betaQuestWorld;

            if (!isBetaQuestEnabled) {
                UIWidgets::DisableComponent(ImGui::GetStyle().Alpha * 0.5f);
            }

            UIWidgets::CVarCheckbox("Enable Beta Quest", "gEnableBetaQuest", {
                .color = colorChoice, 
                .tooltip = "Turns on OoT Beta Quest. *WARNING* This will reset your game.",
            });
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
    if (UIWidgets::BeginMenu("Developer Tools", colorChoice)) {
        ImGui::BeginDisabled(CVarGetInteger("gDisableChangingSettings", 0));
        UIWidgets::CVarCheckbox("OoT Debug Mode", "gDebugEnabled", {
            .color = colorChoice, 
            .tooltip = "Enables Debug Mode, allowing you to select maps with L + R + Z, noclip with L + D-pad Right, and open the debug menu with L on the pause screen.",
        });
        if (CVarGetInteger("gDebugEnabled", 0)) {
            UIWidgets::CVarCheckbox("OoT Registry Editor", "gRegEditEnabled", {
                .color = colorChoice, 
                .tooltip = "Enables the registry editor.",
            });
            UIWidgets::CVarCombobox("Debug Save File Behavior", "gDebugSaveFileMode", DebugSaveFileModes, {
                .color = colorChoice,
                .tooltip = "Changes the behaviour of debug file select creation (creating a save file on slot 1 with debug mode on)\n"
                           "- Off: The debug save file will be a normal savefile\n"
                           "- Vanilla: The debug save file will be the debug save file from the original game\n"
                           "- Maxed: The debug save file will be a save file with all of the items & upgrades",
                .defaultIndex = 0,
            });
            UIWidgets::CVarCheckbox("OoT Skulltula Debug", "gSkulltulaDebugEnabled", {
                .color = colorChoice, 
                .tooltip = "Enables Skulltula Debug, when moving the cursor in the menu above various map icons (boss key, compass, "
                           "map screen locations, etc) will set the GS bits in that area.\nUSE WITH CAUTION AS IT DOES NOT UPDATE THE GS COUNT.",
            });
            UIWidgets::CVarCheckbox("Better Debug Warp Screen", "gBetterDebugWarpScreen", {
                .color = colorChoice, 
                .tooltip = "Optimized debug warp screen, with the added ability to chose entrances and time of day.",
            });
            UIWidgets::CVarCheckbox("Debug Warp Screen Translation", "gDebugWarpScreenTranslation", {
                .color = colorChoice, 
                .tooltip = "Translate the Debug Warp Screen based on the game language.",
            });
            UIWidgets::PaddedSeparator();
        }
        UIWidgets::CVarCheckbox("Fast File Select", "gSkipLogoTitle", {
            .color = colorChoice, 
            .tooltip = "Load the game to the selected menu or file\n\"Zelda Map Select\" require debug mode else you will fallback to File choose menu."
                       "Using a file number that don't have save will create a save file only if you toggle on \"Create a new save if none ?\" else "
                       "it will bring you to the File choose menu.",
        });
        if (CVarGetInteger("gSkipLogoTitle", 0)) {
            UIWidgets::CVarCombobox("Loaded Save File", "gSaveFileID", FastFileSelect, {
                .color = colorChoice,
                .tooltip = "Selects which File is automatically loaded.",
                .defaultIndex = 0,
            });
        }
        
        if (gPlayState != NULL) {
            UIWidgets::PaddedSeparator();
            UIWidgets::CVarCheckbox("Frame Advance", "gFrameAdvanceCtx", {
                .color = colorChoice, 
                .tooltip = "Enable Freeze Frames.",
            });
            if (CVarGetInteger("gFrameAdvanceCtx", 0)) {
                gPlayState->frameAdvCtx.enabled = true;
            } else {
                gPlayState->frameAdvCtx.enabled = false;
            }
            if (gPlayState->frameAdvCtx.enabled) {
                if (UIWidgets::Button("Advance 1", {
                   .color = colorChoice,
                   .size = UIWidgets::Sizes::Inline,
                   .tooltip = "Advance 1 Frame.",
                })) {
                    CVarSetInteger("gFrameAdvance", 1);
                }
                ImGui::SameLine(0, 2.0f);
                UIWidgets::Button("Advance (Hold)", {
                   .color = colorChoice,
                   .size = UIWidgets::Sizes::Inline,
                   .tooltip = "Advance 1 Frame.",
                });
                if (ImGui::IsItemActive()) {
                    CVarSetInteger("gFrameAdvance", 1);
                }                
            }
        }
        UIWidgets::PaddedSeparator();
        if (mStatsWindow) {
            UIWidgets::WindowButton("Stats", "gStatsEnabled", mStatsWindow, {
                .color = colorChoice,
                .tooltip = "Shows the stats window, with your FPS and frametimes, and the OS you're playing on.",
            });
        }
        if (mConsoleWindow) {
            UIWidgets::WindowButton("Console", "gConsoleEnabled", mConsoleWindow, {
                .color = colorChoice,
                .tooltip = "Enables the console window, allowing you to input commands, type help for some examples.",
            });
        }
        if (mSaveEditorWindow) {
            UIWidgets::WindowButton("Save Editor", "gSaveEditorEnabled", mSaveEditorWindow, {
                .color = colorChoice,
                .tooltip = "Enables the console window, allowing you to input commands, type help for some examples.",
            });
        }
        if (mColViewerWindow) {
            UIWidgets::WindowButton("Collision Viewer", "gCollisionViewerEnabled", mColViewerWindow, {
                .color = colorChoice,
                .tooltip = "Opens the Collision Viewer window to display various colliders.",
            });
        }
        if (mActorViewerWindow) {
            UIWidgets::WindowButton("Actor Viewer", "gActorViewerEnabled", mActorViewerWindow, {
                .color = colorChoice,
                .tooltip = "Opens the Actor Viewer window to display information about loaded Actors.",
            });
        }
        if (mDLViewerWindow) {
            UIWidgets::WindowButton("Display List Viewer", "gDLViewerEnabled", mDLViewerWindow, {
                .color = colorChoice,
                .tooltip = "Opens the Display List Viewer window to display information about loaded DLs.",
            });
        }
        if (mValueViewerWindow) {
            UIWidgets::WindowButton("Value Viewer", "gValueViewer.WindowOpen", mValueViewerWindow, {
                .color = colorChoice,
                .tooltip = "Opens the Value Viewer window.",
            });
        }
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
    if (UIWidgets::BeginMenu("Randomizer", colorChoice)) {
    #ifdef __WIIU__
        static ImVec2 buttonSize(200.0f * 2.0f, 0.0f);
    #else
        static ImVec2 buttonSize(200.0f, 0.0f);
    #endif
        if (mRandomizerSettingsWindow) {
            UIWidgets::WindowButton("Randomizer Settings", "gRandomizerSettingsEnabled", mRandomizerSettingsWindow, {
                .color = colorChoice,
                .tooltip = "Opens/Closes the Randomizer Settings Menu.",
            });
        }
        if (mItemTrackerWindow) {
            UIWidgets::WindowButton("Item Tracker", "gItemTrackerEnabled", mItemTrackerWindow, {
                .color = colorChoice,
                .tooltip = "Opens/Closes the Item Tracker Overlay.",
            });
        }
        if (mItemTrackerSettingsWindow) {
            UIWidgets::WindowButton("Item Tracker Settings", "gItemTrackerSettingsEnabled", mItemTrackerSettingsWindow, {
                .color = colorChoice,
                .tooltip = "Opens/Closes the Item Tracker Settings Menu.",
            });
        }
        if (mEntranceTrackerWindow) {
            UIWidgets::WindowButton("Entrance Tracker", "gEntranceTrackerEnabled", mEntranceTrackerWindow, {
                .color = colorChoice,
                .tooltip = "Opens/Closes the Entrance Tracker Overlay.",
            });
        }
        if (mCheckTrackerWindow) {
            UIWidgets::WindowButton("Check Tracker", "gCheckTrackerEnabled", mCheckTrackerWindow, {
                .color = colorChoice,
                .tooltip = "Opens/Closes the Check Tracker Overlay.",
            });
        }
        if (mCheckTrackerSettingsWindow) {
            UIWidgets::WindowButton("Check Tracker Settings", "gCheckTrackerSettingsEnabled", mCheckTrackerSettingsWindow, {
                .color = colorChoice,
                .tooltip = "Opens/Closes the Check Tracker Overlay.",
            });
        }
        UIWidgets::PaddedSeparator();

        if (UIWidgets::BeginMenu("Rando Enhancements", colorChoice))
        {
            UIWidgets::CVarCheckbox("Rando-Relevant Navi Hints", "gRandoRelevantNavi", {
                .color = colorChoice, 
                .tooltip = "Replace Navi's overworld quest hints with rando-related gameplay hints.",
            });
            UIWidgets::CVarCheckbox("Random Rupee Names", "gRandomizeRupeeNames", {
                .color = colorChoice, 
                .tooltip = "When obtaining rupees, randomize what the rupee is called in the textbox.",
            });

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

            UIWidgets::CVarCheckbox("Key Colors Match Dungeon", "gRandoMatchKeyColors", {
                .color = colorChoice, 
                .tooltip = "Matches the color of small keys and boss keys to the dungeon they belong to. \n"
                           "This helps identify keys from afar and adds a little bit of flair.\nThis only "
                           "applies to seeds with keys and boss keys shuffled to Any Dungeon, Overworld, or Anywhere.",
                .disabled = disableKeyColors,
                .disabledTooltip = disableKeyColorsText,
            });
            UIWidgets::CVarCheckbox("Quest Item Fanfares", "gRandoQuestItemFanfares", {
                .color = colorChoice, 
                .tooltip = "Play unique fanfares when obtaining quest items (medallions/stones/songs). Note that these fanfares are longer than usual.",
            });
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void ColorSelector() {
    switch (CVarGetInteger("gMenuTheme", 0)) {
        case ColorOption::GRAY:
            colorChoice = UIWidgets::Colors::Gray;
            break;
        case ColorOption::INDIGO:
            colorChoice = UIWidgets::Colors::Indigo;
            break;
        case ColorOption::RED:
            colorChoice = UIWidgets::Colors::Red;
            break;
        case ColorOption::DARK_RED:
            colorChoice = UIWidgets::Colors::DarkRed;
            break;
        case ColorOption::LIGHT_GREEN:
            colorChoice = UIWidgets::Colors::LightGreen;
            break;
        case ColorOption::GREEN:
            colorChoice = UIWidgets::Colors::Green;
            break;
        case ColorOption::DARK_GREEN:
            colorChoice = UIWidgets::Colors::DarkGreen;
            break;
        case ColorOption::YELLOW:
            colorChoice = UIWidgets::Colors::Yellow;
            break;
        case ColorOption::BLUE:
            colorChoice = UIWidgets::Colors::Blue;
            break;
        case ColorOption::PURPLE:
            colorChoice = UIWidgets::Colors::Purple;
            break;
        default:
            colorChoice = UIWidgets::Colors::White;
            break;
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
    ColorSelector();
}
} // namespace SohGui 
