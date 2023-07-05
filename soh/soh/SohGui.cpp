//
//  SohGui.cpp
//  soh
//
//  Created by David Chavez on 24.08.22.
//

#include "SohGui.hpp"

#include <spdlog/spdlog.h>
#include <ImGui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui_internal.h>
#include <libultraship/libultraship.h>
#include <Fast3D/gfx_pc.h>

#ifdef __APPLE__
#include "graphic/Fast3D/gfx_metal.h"
#endif

#ifdef __SWITCH__
#include <port/switch/SwitchImpl.h>
#endif

#include "UIWidgets.hpp"
#include "include/global.h"
#include "include/z64audio.h"
#include "soh/SaveManager.h"
#include "OTRGlobals.h"
#include "soh/Enhancements/presets.h"
#include "soh/resource/type/Skeleton.h"
#include "libultraship/libultraship.h"

#ifdef ENABLE_REMOTE_CONTROL
#include "Enhancements/crowd-control/CrowdControl.h"
#include "Enhancements/game-interactor/GameInteractor_BuiltIn.h"
#endif

#include "Enhancements/game-interactor/GameInteractor.h"
#include "Enhancements/cosmetics/authenticGfxPatches.h"

bool ShouldClearTextureCacheAtEndOfFrame = false;
bool isBetaQuestEnabled = false;

extern "C" {
    void enableBetaQuest() { isBetaQuestEnabled = true; }
    void disableBetaQuest() { isBetaQuestEnabled = false; }
}


namespace SohGui {

    // MARK: - Properties

    static const char* chestSizeAndTextureMatchesContentsOptions[4] = { "Disabled", "Both", "Texture Only", "Size Only" };
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
    static const char* zFightingOptions[3] = { "Disabled", "Consistent Vanish", "No Vanish" };
    static const char* autosaveLabels[6] = { "Off", "New Location + Major Item", "New Location + Any Item", "New Location", "Major Item", "Any Item" };
    static const char* FastFileSelect[5] = { "File N.1", "File N.2", "File N.3", "Zelda Map Select (require OoT Debug Mode)", "File select" };
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

    static const inline std::vector<std::pair<const char*, const char*>> audioBackends = {
#ifdef _WIN32
            { "wasapi", "Windows Audio Session API" },
#endif
#if defined(__linux)
            { "pulse", "PulseAudio" },
#endif
            { "sdl", "SDL Audio" }
    };


    // MARK: - Helpers

    std::string GetWindowButtonText(const char* text, bool menuOpen) {
        char buttonText[100] = "";
        if (menuOpen) {
            strcat(buttonText, ICON_FA_CHEVRON_RIGHT " ");
        }
        strcat(buttonText, text);
        if (!menuOpen) { strcat(buttonText, "  "); }
        return buttonText;
    }


    // MARK: - Delegates

    std::shared_ptr<SohMenuBar> mSohMenuBar;

    std::shared_ptr<LUS::GuiWindow> mConsoleWindow;
    std::shared_ptr<LUS::GuiWindow> mStatsWindow;
    std::shared_ptr<LUS::GuiWindow> mInputEditorWindow;

    std::shared_ptr<AudioEditor> mAudioEditorWindow;
    std::shared_ptr<GameControlEditor::GameControlEditorWindow> mGameControlEditorWindow;
    std::shared_ptr<CosmeticsEditorWindow> mCosmeticsEditorWindow;
    std::shared_ptr<ActorViewerWindow> mActorViewerWindow;
    std::shared_ptr<ColViewerWindow> mColViewerWindow;
    std::shared_ptr<SaveEditorWindow> mSaveEditorWindow;
    std::shared_ptr<DLViewerWindow> mDLViewerWindow;
    std::shared_ptr<GameplayStatsWindow> mGameplayStatsWindow;
    std::shared_ptr<CheckTracker::CheckTrackerSettingsWindow> mCheckTrackerSettingsWindow;
    std::shared_ptr<CheckTracker::CheckTrackerWindow> mCheckTrackerWindow;
    std::shared_ptr<EntranceTrackerWindow> mEntranceTrackerWindow;
    std::shared_ptr<ItemTrackerSettingsWindow> mItemTrackerSettingsWindow;
    std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
    std::shared_ptr<RandomizerSettingsWindow> mRandomizerSettingsWindow;

    void SetupGuiElements() {
        auto gui = LUS::Context::GetInstance()->GetWindow()->GetGui();

        mSohMenuBar = std::make_shared<SohMenuBar>("gOpenMenuBar", CVarGetInteger("gOpenMenuBar", 0));
        gui->SetMenuBar(std::reinterpret_pointer_cast<LUS::GuiMenuBar>(mSohMenuBar));

        if (gui->GetMenuBar() && !gui->GetMenuBar()->IsVisible()) {
#if defined(__SWITCH__) || defined(__WIIU__)
            gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Press - to access enhancements menu");
#else
            gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Press F1 to access enhancements menu");
#endif
        }

        mStatsWindow = gui->GetGuiWindow("Stats");
        if (mStatsWindow == nullptr) {
            SPDLOG_ERROR("Could not find stats window");
        }

        mConsoleWindow = gui->GetGuiWindow("Console");
        if (mConsoleWindow == nullptr) {
            SPDLOG_ERROR("Could not find console window");
        }

        mInputEditorWindow = gui->GetGuiWindow("Input Editor");
        if (mInputEditorWindow == nullptr) {
            SPDLOG_ERROR("Could not find input editor window");
        }

        mAudioEditorWindow = std::make_shared<AudioEditor>("gAudioEditor.WindowOpen", "Audio Editor");
        gui->AddGuiWindow(mAudioEditorWindow);
        mGameControlEditorWindow = std::make_shared<GameControlEditor::GameControlEditorWindow>("gGameControlEditorEnabled", "Game Control Editor");
        gui->AddGuiWindow(mGameControlEditorWindow);
        mCosmeticsEditorWindow = std::make_shared<CosmeticsEditorWindow>("gCosmeticsEditorEnabled", "Cosmetics Editor");
        gui->AddGuiWindow(mCosmeticsEditorWindow);
        mActorViewerWindow = std::make_shared<ActorViewerWindow>("gActorViewerEnabled", "Actor Viewer");
        gui->AddGuiWindow(mActorViewerWindow);
        mColViewerWindow = std::make_shared<ColViewerWindow>("gCollisionViewerEnabled", "Collision Viewer");
        gui->AddGuiWindow(mColViewerWindow);
        mSaveEditorWindow = std::make_shared<SaveEditorWindow>("gSaveEditorEnabled", "Save Editor");
        gui->AddGuiWindow(mSaveEditorWindow);
        mDLViewerWindow = std::make_shared<DLViewerWindow>("gDLViewerEnabled", "Display List Viewer");
        gui->AddGuiWindow(mDLViewerWindow);
        mGameplayStatsWindow = std::make_shared<GameplayStatsWindow>("gGameplayStatsEnabled", "Gameplay Stats");
        gui->AddGuiWindow(mGameplayStatsWindow);
        mCheckTrackerWindow = std::make_shared<CheckTracker::CheckTrackerWindow>("gCheckTrackerEnabled", "Check Tracker");
        gui->AddGuiWindow(mCheckTrackerWindow);
        mCheckTrackerSettingsWindow = std::make_shared<CheckTracker::CheckTrackerSettingsWindow>("gCheckTrackerSettingsEnabled", "Check Tracker Settings");
        gui->AddGuiWindow(mCheckTrackerSettingsWindow);
        mEntranceTrackerWindow = std::make_shared<EntranceTrackerWindow>("gEntranceTrackerEnabled","Entrance Tracker");
        gui->AddGuiWindow(mEntranceTrackerWindow);
        mItemTrackerWindow = std::make_shared<ItemTrackerWindow>("gItemTrackerEnabled", "Item Tracker");
        gui->AddGuiWindow(mItemTrackerWindow);
        mItemTrackerSettingsWindow = std::make_shared<ItemTrackerSettingsWindow>("gItemTrackerSettingsEnabled", "Item Tracker Settings");
        gui->AddGuiWindow(mItemTrackerSettingsWindow);
        mRandomizerSettingsWindow = std::make_shared<RandomizerSettingsWindow>("gRandomizerSettingsEnabled", "Randomizer Settings");
        gui->AddGuiWindow(mRandomizerSettingsWindow);
    }

    void Destroy() {
        mRandomizerSettingsWindow = nullptr;
        mItemTrackerWindow = nullptr;
        mItemTrackerSettingsWindow = nullptr;
        mEntranceTrackerWindow = nullptr;
        mCheckTrackerWindow = nullptr;
        mCheckTrackerSettingsWindow = nullptr;
        mGameplayStatsWindow = nullptr;
        mDLViewerWindow = nullptr;
        mSaveEditorWindow = nullptr;
        mColViewerWindow = nullptr;
        mActorViewerWindow = nullptr;
        mCosmeticsEditorWindow = nullptr;
        mGameControlEditorWindow = nullptr;
        mAudioEditorWindow = nullptr;
        mInputEditorWindow = nullptr;
        mStatsWindow = nullptr;
        mConsoleWindow = nullptr;
        mSohMenuBar = nullptr;
    }
}
