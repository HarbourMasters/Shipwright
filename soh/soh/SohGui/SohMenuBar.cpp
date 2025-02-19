#include "SohMenuBar.h"
#include <imgui.h>
#include "regex"
#include "public/bridge/consolevariablebridge.h"
#include <libultraship/libultraship.h>
#include "UIWidgets.hpp"
#include "include/z64audio.h"
#include "graphic/Fast3D/gfx_rendering_api.h"
#include "soh/OTRGlobals.h"
#include "soh/SaveManager.h"
#include "z64.h"
#include "soh/cvar_prefixes.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/presets.h"
#include "soh/Enhancements/mods.h"
#include "soh/Notification/Notification.h"
#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#ifdef ENABLE_REMOTE_CONTROL
#include "soh/Network/CrowdControl/CrowdControl.h"
#include "soh/Network/Sail/Sail.h"
#endif


#include "soh/Enhancements/audio/AudioEditor.h"
#include "soh/Enhancements/controls/InputViewer.h"
#include "soh/Enhancements/cosmetics/CosmeticsEditor.h"
#include "soh/Enhancements/debugger/actorViewer.h"
#include "soh/Enhancements/debugger/colViewer.h"
#include "soh/Enhancements/debugger/debugSaveEditor.h"
#include "soh/Enhancements/debugger/hookDebugger.h"
#include "soh/Enhancements/debugger/dlViewer.h"
#include "soh/Enhancements/debugger/valueViewer.h"
#include "soh/Enhancements/gameplaystatswindow.h"
#include "soh/Enhancements/debugger/MessageViewer.h"
#include "soh/Enhancements/randomizer/randomizer_check_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_item_tracker.h"
#include "soh/Enhancements/randomizer/randomizer_settings_window.h"
#include "soh/Enhancements/resolution-editor/ResolutionEditor.h"
#include "soh/Enhancements/enemyrandomizer.h"
#include "soh/Enhancements/timesplits/TimeSplits.h"
#include "soh/Enhancements/randomizer/Plandomizer.h"
#include "soh/Enhancements/TimeDisplay/TimeDisplay.h"
#include "soh/Enhancements/Autosave.h"

// FA icons are kind of wonky, if they worked how I expected them to the "+ 2.0f" wouldn't be needed, but
// they don't work how I expect them to so I added that because it looked good when I eyeballed it
#define FA_ICON_BUTTON_FRAME_PADDING_X(icon) (((optionsButtonSize.x - ImGui::CalcTextSize(icon).x) / 2) + 2.0f)

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
    static const char* skipGetItemAnimationOptions[3] = { "Disabled", "Junk Items", "All Items" };
    static const char* skipForcedDialogOptions[4] = { "None", "Navi Only", "NPCs Only", "All" };
    static const char* sleepingWaterfallOptions[3] = { "Always", "Once", "Never" };
    static const char* bunnyHoodOptions[3] = { "Disabled", "Faster Run & Longer Jump", "Faster Run" };
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
    static const char* bootSequenceLabels[3] = { "Default", "Authentic", "File Select" };
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
    static const char* itemCountMessageCVars[3] = {
        CVAR_ENHANCEMENT("InjectItemCounts.GoldSkulltula"),
        CVAR_ENHANCEMENT("InjectItemCounts.HeartPiece"),
        CVAR_ENHANCEMENT("InjectItemCounts.HeartContainer"),
    };
    static const char* itemCountMessageOptions[ARRAY_COUNT(itemCountMessageCVars)] = {
        "Gold Skulltula Tokens",
        "Pieces of Heart",
        "Heart Containers",
    };

extern "C" SaveContext gSaveContext;

namespace SohGui {

std::unordered_map<Ship::WindowBackend, const char*> availableWindowBackendsMap;
Ship::WindowBackend configWindowBackend;

extern std::shared_ptr<Ship::GuiWindow> mGfxDebuggerWindow;
extern std::shared_ptr<AdvancedResolutionSettings::AdvancedResolutionSettingsWindow> mAdvancedResolutionSettingsWindow;

void DrawSettingsMenu() {
    if (ImGui::BeginMenu("Settings"))
    {
        if (ImGui::BeginMenu("Graphics")) {
        #ifndef __APPLE__
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
        #endif
            ImGui::Text("ImGui Menu Scale");
            ImGui::SameLine();
            ImGui::TextColored({ 0.85f, 0.35f, 0.0f, 1.0f }, "(Experimental)");
            if (UIWidgets::EnhancementCombobox(CVAR_SETTING("ImGuiScale"), imguiScaleOptions, 1)) {
                OTRGlobals::Instance->ScaleImGui();
            }
            UIWidgets::Tooltip("Changes the scaling of the ImGui menu elements.");

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

extern std::shared_ptr<TimeDisplayWindow> mTimeDisplayWindow;

void DrawEnhancementsMenu() {
    if (ImGui::BeginMenu("Enhancements"))
    {
        ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));

        DrawPresetSelector(PRESET_TYPE_ENHANCEMENTS);

        UIWidgets::PaddedSeparator();

        UIWidgets::EnhancementCheckbox("Autosave", CVAR_ENHANCEMENT("Autosave"));
        UIWidgets::Tooltip("Save the game automatically on a 3 minute interval and when soft-resetting the game.\n\n"
                           "The interval autosave will wait if the game is paused in any way (dialogue, pause screen up, cutscenes).");

        UIWidgets::PaddedSeparator(true, true, 2.0f, 2.0f);

        UIWidgets::PaddedText("Boot Sequence", false, true);
        UIWidgets::EnhancementCombobox(CVAR_ENHANCEMENT("BootSequence"), bootSequenceLabels, BOOTSEQUENCE_DEFAULT);
        UIWidgets::Tooltip("Configure what happens when starting or resetting the game\n\n"
                           "Default: LUS logo -> N64 logo\n"
                           "Authentic: N64 logo only\n"
                           "File Select: Skip to file select menu"
        );

        UIWidgets::PaddedSeparator(true, true, 2.0f, 2.0f);

        ImGui::EndDisabled();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));

        if (mTimeDisplayWindow) {
            if (ImGui::Button(GetWindowButtonText("Additional Timers", CVarGetInteger(CVAR_WINDOW("TimeDisplayEnabled"), 0)).c_str(), ImVec2(-1.0f, 0.0f))) {
                mTimeDisplayWindow->ToggleVisibility();
            }
        }
        if (mTimeDisplayWindow->IsVisible()) {
            ImGui::SeparatorText("Timer Display Options");

            if (!gPlayState) {
                ImGui::Text("Additional Timer options\n"
                            "available when a file is\n"
                            "loaded...");
            } else {
                if (UIWidgets::PaddedEnhancementSliderFloat("Font Scale: %.2fx", "##FontScale", CVAR_ENHANCEMENT("TimeDisplay.FontScale"), 
                    1.0f, 5.0f, "", 1.0f, false, true, false, true)) {
                    TimeDisplayInitSettings();
                }
                if (UIWidgets::PaddedEnhancementCheckbox("Hide Background", CVAR_ENHANCEMENT("TimeDisplay.ShowWindowBG"), 
                    false, false)) {
                    TimeDisplayInitSettings();
                }
                ImGui::Separator();
                for (auto& timer : timeDisplayList) {
                    if (UIWidgets::PaddedEnhancementCheckbox(timer.timeLabel.c_str(), timer.timeEnable, false, false)) {
                        TimeDisplayUpdateDisplayOptions();
                    }
                }
            }
        }
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(1);

        ImGui::EndMenu();
    }
}

#ifdef ENABLE_REMOTE_CONTROL
void DrawRemoteControlMenu() {
    if (ImGui::BeginMenu("Network")) {
        Sail::Instance->DrawMenu();
        CrowdControl::Instance->DrawMenu();
        ImGui::EndMenu();
    }
}
#endif

extern "C" u8 Randomizer_GetSettingValue(RandomizerSettingKey randoSettingKey);

void DrawRandomizerMenu() {
    if (ImGui::BeginMenu("Randomizer")) {

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
            
            UIWidgets::PaddedEnhancementCheckbox("Use Custom Key Models", CVAR_RANDOMIZER_ENHANCEMENT("CustomKeyModels"), true, false);
            UIWidgets::Tooltip("Use Custom graphics for dungeon keys, Big and Small, so that they can be easily told apart");

            bool disableCompassColors = !DUNGEON_ITEMS_CAN_BE_OUTSIDE_DUNGEON(RSK_SHUFFLE_MAPANDCOMPASS);

            static const char* disableCompassColorsText =
                "This setting is disabled because a savefile is loaded without the compass\n"
                "shuffle settings set to \"Any Dungeon\", \"Overworld\" or \"Anywhere\"";

            if (UIWidgets::PaddedEnhancementCheckbox("Compass Colors Match Dungeon", CVAR_RANDOMIZER_ENHANCEMENT("MatchCompassColors"), true, false,
                                                  disableCompassColors, disableCompassColorsText, UIWidgets::CheckboxGraphics::Cross, true)) {
                PatchCompasses();
            }
            UIWidgets::Tooltip(
                "Matches the color of compasses to the dungeon they belong to. "
                "This helps identify compasses from afar and adds a little bit of flair.\n\nThis only "
                "applies to seeds with compasses shuffled to \"Any Dungeon\", \"Overworld\", or \"Anywhere\".");

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
            UIWidgets::PaddedEnhancementCheckbox("Simpler Boss Soul Models",
                                                 CVAR_RANDOMIZER_ENHANCEMENT("SimplerBossSoulModels"), true, false);
            UIWidgets::Tooltip(
                "When shuffling boss souls, they'll appear as a simpler model instead of showing the boss' models."
                "This might make boss souls more distinguishable from a distance, and can help with performance."
            );
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void SohMenuBar::InitElement() {
    
}

void SohMenuBar::DrawElement() {
    if (ImGui::BeginMenuBar()) {
        static ImVec2 sWindowPadding(8.0f, 8.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sWindowPadding);

        DrawSettingsMenu();

        ImGui::SetCursorPosY(0.0f);

        DrawEnhancementsMenu();

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
