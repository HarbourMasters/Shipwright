//
//  SohGui.cpp
//  soh
//
//  Created by David Chavez on 24.08.22.
//

#include "SohGui.hpp"

#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <libultraship/libultraship.h>
#include <Fast3D/gfx_pc.h>

#ifdef __APPLE__
#include "graphic/Fast3D/gfx_metal.h"
#endif

#ifdef __SWITCH__
#include <port/switch/SwitchImpl.h>
#endif
#include "SohMenu.h"
#include "include/global.h"
#include "include/z64audio.h"
#include "soh/SaveManager.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/Presets/Presets.h"
#include "soh/resource/type/Skeleton.h"
#include "libultraship/libultraship.h"

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/cosmetics/authenticGfxPatches.h"
#include "soh/Enhancements/debugger/MessageViewer.h"
#include "soh/Notification/Notification.h"
#include "soh/Enhancements/TimeDisplay/TimeDisplay.h"

namespace SohGui {

    // MARK: - Properties
    static const char* bunnyHoodOptions[3] = { "Disabled", "Faster Run & Longer Jump", "Faster Run" };

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

    std::shared_ptr<Ship::GuiWindow> mConsoleWindow;
    std::shared_ptr<SohStatsWindow> mStatsWindow;
    std::shared_ptr<Ship::GuiWindow> mGfxDebuggerWindow;
    std::shared_ptr<Ship::GuiWindow> mInputEditorWindow;
    
    std::shared_ptr<SohMenu> mSohMenu;
    std::shared_ptr<AudioEditor> mAudioEditorWindow;
    std::shared_ptr<InputViewer> mInputViewer;
    std::shared_ptr<InputViewerSettingsWindow> mInputViewerSettings;
    std::shared_ptr<CosmeticsEditorWindow> mCosmeticsEditorWindow;
    std::shared_ptr<ActorViewerWindow> mActorViewerWindow;
    std::shared_ptr<ColViewerWindow> mColViewerWindow;
    std::shared_ptr<SaveEditorWindow> mSaveEditorWindow;
    std::shared_ptr<HookDebuggerWindow> mHookDebuggerWindow;
    std::shared_ptr<DLViewerWindow> mDLViewerWindow;
    std::shared_ptr<ValueViewerWindow> mValueViewerWindow;
    std::shared_ptr<MessageViewer> mMessageViewerWindow;
    std::shared_ptr<GameplayStatsWindow> mGameplayStatsWindow;
    std::shared_ptr<CheckTracker::CheckTrackerSettingsWindow> mCheckTrackerSettingsWindow;
    std::shared_ptr<CheckTracker::CheckTrackerWindow> mCheckTrackerWindow;
    std::shared_ptr<EntranceTrackerSettingsWindow> mEntranceTrackerSettingsWindow;
    std::shared_ptr<EntranceTrackerWindow> mEntranceTrackerWindow;
    std::shared_ptr<ItemTrackerSettingsWindow> mItemTrackerSettingsWindow;
    std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
    std::shared_ptr<TimeSplitWindow> mTimeSplitWindow;
    std::shared_ptr<PlandomizerWindow> mPlandomizerWindow;
    std::shared_ptr<RandomizerSettingsWindow> mRandomizerSettingsWindow;
    std::shared_ptr<SohModalWindow> mModalWindow;
    std::shared_ptr<Notification::Window> mNotificationWindow;
    std::shared_ptr<TimeDisplayWindow> mTimeDisplayWindow;

    UIWidgets::Colors GetMenuThemeColor() {
        return mSohMenu->GetMenuThemeColor();
    }

    void SetupGuiElements() {
        auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();

        /*mSohMenuBar = std::make_shared<SohMenuBar>(CVAR_MENU_BAR_OPEN, CVarGetInteger(CVAR_MENU_BAR_OPEN, 0));
        gui->SetMenuBar(std::reinterpret_pointer_cast<Ship::GuiMenuBar>(mSohMenuBar));

        if (!gui->GetMenuBar() && !CVarGetInteger("gSettings.DisableMenuShortcutNotify", 0)) {
#if defined(__SWITCH__) || defined(__WIIU__)
            gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Press - to access enhancements menu");
#else
            gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Press F1 to access enhancements menu");
            gui->GetGameOverlay()->TextDrawNotification(30.0f, true, "Press F2 to enable the mouse cursor");
#endif
        }*/

        mSohMenu = std::make_shared<SohMenu>(CVAR_WINDOW("Menu"), "Port Menu");
        gui->SetMenu(mSohMenu);

        mConsoleWindow = std::make_shared<SohConsoleWindow>(CVAR_WINDOW("SohConsole"), "Console##SoH", ImVec2(820, 630));
        gui->AddGuiWindow(mConsoleWindow);

        mGfxDebuggerWindow = std::make_shared<SohGfxDebuggerWindow>(CVAR_WINDOW("SohGfxDebugger"), "GfxDebugger##SoH", ImVec2(820, 630));
        gui->AddGuiWindow(mGfxDebuggerWindow);

        mStatsWindow = std::make_shared<SohStatsWindow>(CVAR_WINDOW("SohStats"), "Stats##Soh", ImVec2(400, 100));
        gui->AddGuiWindow(mStatsWindow);

        mInputEditorWindow = gui->GetGuiWindow("Controller Configuration");
        if (mInputEditorWindow == nullptr) {
            SPDLOG_ERROR("Could not find input editor window");
        }

        mAudioEditorWindow = std::make_shared<AudioEditor>(CVAR_WINDOW("AudioEditor"), "Audio Editor", ImVec2(820, 630));
        gui->AddGuiWindow(mAudioEditorWindow);
        mInputViewer = std::make_shared<InputViewer>(CVAR_WINDOW("InputViewer"), "Input Viewer");
        gui->AddGuiWindow(mInputViewer);
        mInputViewerSettings = std::make_shared<InputViewerSettingsWindow>(CVAR_WINDOW("InputViewerSettings"), "Input Viewer Settings", ImVec2(500, 525));
        gui->AddGuiWindow(mInputViewerSettings);
        mCosmeticsEditorWindow = std::make_shared<CosmeticsEditorWindow>(CVAR_WINDOW("CosmeticsEditor"), "Cosmetics Editor", ImVec2(550, 520));
        gui->AddGuiWindow(mCosmeticsEditorWindow);
        mActorViewerWindow = std::make_shared<ActorViewerWindow>(CVAR_WINDOW("ActorViewer"), "Actor Viewer", ImVec2(520, 600));
        gui->AddGuiWindow(mActorViewerWindow);
        mColViewerWindow = std::make_shared<ColViewerWindow>(CVAR_WINDOW("CollisionViewer"), "Collision Viewer", ImVec2(520, 600));
        gui->AddGuiWindow(mColViewerWindow);
        mSaveEditorWindow = std::make_shared<SaveEditorWindow>(CVAR_WINDOW("SaveEditor"), "Save Editor", ImVec2(520, 600));
        gui->AddGuiWindow(mSaveEditorWindow);
        mHookDebuggerWindow = std::make_shared<HookDebuggerWindow>(CVAR_WINDOW("HookDebugger"), "Hook Debugger", ImVec2(1250, 850));
        gui->AddGuiWindow(mHookDebuggerWindow);
        mDLViewerWindow = std::make_shared<DLViewerWindow>(CVAR_WINDOW("DisplayListViewer"), "Display List Viewer", ImVec2(520, 600));
        gui->AddGuiWindow(mDLViewerWindow);
        mValueViewerWindow = std::make_shared<ValueViewerWindow>(CVAR_WINDOW("ValueViewer"), "Value Viewer", ImVec2(520, 600));
        gui->AddGuiWindow(mValueViewerWindow);
        mMessageViewerWindow = std::make_shared<MessageViewer>(CVAR_WINDOW("MessageViewer"), "Message Viewer", ImVec2(520, 600));
        gui->AddGuiWindow(mMessageViewerWindow);
        mGameplayStatsWindow = std::make_shared<GameplayStatsWindow>(CVAR_WINDOW("GameplayStats"), "Gameplay Stats", ImVec2(480, 550));
        gui->AddGuiWindow(mGameplayStatsWindow);
        mCheckTrackerWindow = std::make_shared<CheckTracker::CheckTrackerWindow>(CVAR_WINDOW("CheckTracker"), "Check Tracker", ImVec2(400, 540));
        gui->AddGuiWindow(mCheckTrackerWindow);
        mCheckTrackerSettingsWindow = std::make_shared<CheckTracker::CheckTrackerSettingsWindow>(CVAR_WINDOW("CheckTrackerSettings"), "Check Tracker Settings", ImVec2(600, 375));
        gui->AddGuiWindow(mCheckTrackerSettingsWindow);
        mEntranceTrackerWindow = std::make_shared<EntranceTrackerWindow>(CVAR_WINDOW("EntranceTracker"), "Entrance Tracker", ImVec2(500, 750));
        gui->AddGuiWindow(mEntranceTrackerWindow);
        mEntranceTrackerSettingsWindow = std::make_shared<EntranceTrackerSettingsWindow>(CVAR_WINDOW("EntranceTrackerSettings"), "Entrance Tracker Settings", ImVec2(600, 375));
        gui->AddGuiWindow(mEntranceTrackerSettingsWindow);
        mItemTrackerWindow = std::make_shared<ItemTrackerWindow>(CVAR_WINDOW("ItemTracker"), "Item Tracker", ImVec2(350, 600));
        gui->AddGuiWindow(mItemTrackerWindow);
        mItemTrackerSettingsWindow = std::make_shared<ItemTrackerSettingsWindow>(CVAR_WINDOW("ItemTrackerSettings"), "Item Tracker Settings", ImVec2(733, 472));
        gui->AddGuiWindow(mItemTrackerSettingsWindow);
        mRandomizerSettingsWindow = std::make_shared<RandomizerSettingsWindow>(CVAR_WINDOW("RandomizerSettings"), "Randomizer Settings", ImVec2(920, 600));
        gui->AddGuiWindow(mRandomizerSettingsWindow);
        mTimeSplitWindow = std::make_shared<TimeSplitWindow>(CVAR_WINDOW("TimeSplits"), "Time Splits", ImVec2(450, 660));
        gui->AddGuiWindow(mTimeSplitWindow);
        mPlandomizerWindow = std::make_shared<PlandomizerWindow>(CVAR_WINDOW("PlandomizerEditor"), "Plandomizer Editor", ImVec2(850, 760));
        gui->AddGuiWindow(mPlandomizerWindow);
        mModalWindow = std::make_shared<SohModalWindow>(CVAR_WINDOW("ModalWindow"), "Modal Window");
        gui->AddGuiWindow(mModalWindow);
        mModalWindow->Show();
        mNotificationWindow = std::make_shared<Notification::Window>(CVAR_WINDOW("Notifications"), "Notifications Window");
        gui->AddGuiWindow(mNotificationWindow);
        mNotificationWindow->Show();
        mTimeDisplayWindow = std::make_shared<TimeDisplayWindow>(CVAR_WINDOW("TimeDisplayEnabled"), "Additional Timers");
        gui->AddGuiWindow(mTimeDisplayWindow);
    }

    void Destroy() {
        auto gui = Ship::Context::GetInstance()->GetWindow()->GetGui();
        gui->RemoveAllGuiWindows();
        
        mNotificationWindow = nullptr;
        mModalWindow = nullptr;
        mRandomizerSettingsWindow = nullptr;
        mItemTrackerWindow = nullptr;
        mItemTrackerSettingsWindow = nullptr;
        mEntranceTrackerWindow = nullptr;
        mEntranceTrackerSettingsWindow = nullptr;
        mCheckTrackerWindow = nullptr;
        mCheckTrackerSettingsWindow = nullptr;
        mGameplayStatsWindow = nullptr;
        mDLViewerWindow = nullptr;
        mValueViewerWindow = nullptr;
        mMessageViewerWindow = nullptr;
        mSaveEditorWindow = nullptr;
        mHookDebuggerWindow = nullptr;
        mColViewerWindow = nullptr;
        mActorViewerWindow = nullptr;
        mCosmeticsEditorWindow = nullptr;
        mAudioEditorWindow = nullptr;
        mInputEditorWindow = nullptr;
        mStatsWindow = nullptr;
        mConsoleWindow = nullptr;
        mGfxDebuggerWindow = nullptr;
        mSohMenuBar = nullptr;
        mInputViewer = nullptr;
        mInputViewerSettings = nullptr;
        mTimeSplitWindow = nullptr;
        mPlandomizerWindow = nullptr;
        mTimeDisplayWindow = nullptr;
    }

    void RegisterPopup(std::string title, std::string message, std::string button1, std::string button2, std::function<void()> button1callback, std::function<void()> button2callback) {
        mModalWindow->RegisterPopup(title, message, button1, button2, button1callback, button2callback);
    }

    void ShowRandomizerSettingsMenu() {
        mRandomizerSettingsWindow->Show();
    }
}
