#include "BenGui.hpp"

#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <libultraship/window/gui/GfxDebuggerWindow.h>
#include "UIWidgets.hpp"
#include "HudEditor.h"
#include "2s2h/Enhancements/Audio/AudioEditor.h"
#include "2s2h/Enhancements/ModMenu/ModMenu.h"
#include "CosmeticEditor.h"
#include "Notification.h"
#include "2s2h/Rando/CheckTracker/CheckTracker.h"

#ifdef __APPLE__
#include <fast/backends/gfx_metal.h>
#endif

#ifdef __SWITCH__
#include <port/switch/SwitchImpl.h>
#endif

#include "include/global.h"

#include "Enhancements/Trackers/ItemTracker/ItemTracker.h"
#include "Enhancements/Trackers/ItemTracker/ItemTrackerSettings.h"
#include "Enhancements/Trackers/DisplayOverlay.h"
#include "Enhancements/Trackers//TimeSplits/Timesplits.h"
#include "Enhancements/Trackers/TimeSplits/TimesplitsSettings.h"
#include "BenMenu.h"
#include "BenMenuBar.h"
#include "DeveloperTools/HookDebugger.h"
#include "DeveloperTools/SaveEditor.h"
#include "DeveloperTools/ActorViewer.h"
#include "DeveloperTools/CollisionViewer.h"
#include "DeveloperTools/EventLog.h"
#include "DeveloperTools/DLViewer.h"
#include "DeveloperTools/MessageViewer.h"

namespace BenGui {
// MARK: - Delegates

std::shared_ptr<BenMenuBar> mBenMenuBar;

std::shared_ptr<Ship::GuiWindow> mConsoleWindow;
std::shared_ptr<Ship::GuiWindow> mStatsWindow;
std::shared_ptr<Ship::GuiWindow> mGfxDebuggerWindow;
std::shared_ptr<Ship::GuiWindow> mInputEditorWindow;

std::shared_ptr<HookDebuggerWindow> mHookDebuggerWindow;
std::shared_ptr<SaveEditorWindow> mSaveEditorWindow;
std::shared_ptr<HudEditorWindow> mHudEditorWindow;
std::shared_ptr<CosmeticEditorWindow> mCosmeticEditorWindow;
std::shared_ptr<ActorViewerWindow> mActorViewerWindow;
std::shared_ptr<CollisionViewerWindow> mCollisionViewerWindow;
std::shared_ptr<EventLogWindow> mEventLogWindow;
std::shared_ptr<DLViewerWindow> mDLViewerWindow;
std::shared_ptr<MessageViewerWindow> mMessageViewerWindow;
std::shared_ptr<AudioEditor> mAudioEditorWindow;
std::shared_ptr<ModMenuWindow> mModMenuWindow;
std::shared_ptr<BenMenu> mBenMenu;
std::shared_ptr<Notification::Window> mNotificationWindow;
std::shared_ptr<Rando::CheckTracker::CheckTrackerWindow> mRandoCheckTrackerWindow;
std::shared_ptr<Rando::CheckTracker::SettingsWindow> mRandoCheckTrackerSettingsWindow;
std::shared_ptr<ItemTrackerWindow> mItemTrackerWindow;
std::shared_ptr<ItemTrackerSettingsWindow> mItemTrackerSettingsWindow;
std::shared_ptr<DisplayOverlayWindow> mDisplayOverlayWindow;
std::shared_ptr<TimesplitsWindow> mTimesplitsWindow;
std::shared_ptr<TimesplitsSettingsWindow> mTimesplitsSettingsWindow;
std::shared_ptr<InputViewer> mInputViewer;
std::shared_ptr<InputViewerSettingsWindow> mInputViewerSettings;
std::shared_ptr<BenModalWindow> mModalWindow;

UIWidgets::Colors GetMenuThemeColor() {
    return mBenMenu->GetMenuThemeColor();
}

void SetupMenu() {
    auto gui = Ship::Context::GetRawInstance()->GetWindow()->GetGui();
    mBenMenu = std::make_shared<BenMenu>("gWindows.Menu", "Settings Menu");
    gui->SetMenu(mBenMenu);

    auto& style = ImGui::GetStyle();
    style.FramePadding = ImVec2(4.0f, 6.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.Colors[ImGuiCol_MenuBarBg] = UIWidgets::ColorValues.at(UIWidgets::Colors::DarkGray);

    mModalWindow = std::make_shared<BenModalWindow>("gWindows.ModalWindow", "Modal Window");
    gui->AddGuiWindow(mModalWindow);
    mModalWindow->Show();
}

void SetupGuiElements() {
    auto gui = Ship::Context::GetRawInstance()->GetWindow()->GetGui();

    mStatsWindow = gui->GetGuiWindow("Stats");
    if (mStatsWindow == nullptr) {
        SPDLOG_ERROR("Could not find stats window");
    }

    mConsoleWindow = gui->GetGuiWindow("Console");
    if (mConsoleWindow == nullptr) {
        SPDLOG_ERROR("Could not find console window");
    }

    mGfxDebuggerWindow =
        std::make_shared<LUS::GfxDebuggerWindow>("gOpenWindows.GfxDebugger", "Gfx Debugger", ImVec2(520, 600));
    gui->AddGuiWindow(mGfxDebuggerWindow);

    mInputEditorWindow = gui->GetGuiWindow("2S2H Input Editor");
    if (mInputEditorWindow == nullptr) {
        SPDLOG_ERROR("Could not find input editor window");
    }

    mHookDebuggerWindow =
        std::make_shared<HookDebuggerWindow>("gWindows.HookDebugger", "Hook Debugger", ImVec2(480, 600));
    gui->AddGuiWindow(mHookDebuggerWindow);

    mSaveEditorWindow = std::make_shared<SaveEditorWindow>("gWindows.SaveEditor", "Save Editor", ImVec2(480, 600));
    gui->AddGuiWindow(mSaveEditorWindow);

    mHudEditorWindow = std::make_shared<HudEditorWindow>("gWindows.HudEditor", "HUD Editor", ImVec2(480, 600));
    gui->AddGuiWindow(mHudEditorWindow);

    mCosmeticEditorWindow =
        std::make_shared<CosmeticEditorWindow>("gWindows.CosmeticEditor", "Cosmetic Editor", ImVec2(480, 600));
    gui->AddGuiWindow(mCosmeticEditorWindow);

    mActorViewerWindow = std::make_shared<ActorViewerWindow>("gWindows.ActorViewer", "Actor Viewer", ImVec2(520, 600));
    gui->AddGuiWindow(mActorViewerWindow);

    mCollisionViewerWindow =
        std::make_shared<CollisionViewerWindow>("gWindows.CollisionViewer", "Collision Viewer", ImVec2(390, 475));
    gui->AddGuiWindow(mCollisionViewerWindow);

    mEventLogWindow = std::make_shared<EventLogWindow>("gWindows.EventLog", "Event Log", ImVec2(520, 600));
    gui->AddGuiWindow(mEventLogWindow);

    mDLViewerWindow = std::make_shared<DLViewerWindow>("gWindows.DLViewer", "DL Viewer", ImVec2(520, 600));
    gui->AddGuiWindow(mDLViewerWindow);
    mMessageViewerWindow =
        std::make_shared<MessageViewerWindow>("gWindows.MessageViewer", "Message Viewer", ImVec2(520, 600));
    gui->AddGuiWindow(mMessageViewerWindow);

    mAudioEditorWindow = std::make_shared<AudioEditor>("gWindows.AudioEditor", "Audio Editor", ImVec2(520, 600));
    gui->AddGuiWindow(mAudioEditorWindow);

    mModMenuWindow = std::make_shared<ModMenuWindow>("gWindows.ModMenu", "Mod Menu", ImVec2(820, 630));
    gui->AddGuiWindow(mModMenuWindow);

    mItemTrackerWindow = std::make_shared<ItemTrackerWindow>("gWindows.ItemTracker", "Item Tracker");
    gui->AddGuiWindow(mItemTrackerWindow);

    mItemTrackerSettingsWindow = std::make_shared<ItemTrackerSettingsWindow>("gWindows.ItemTrackerSettings",
                                                                             "Item Tracker Settings", ImVec2(800, 400));
    gui->AddGuiWindow(mItemTrackerSettingsWindow);

    mDisplayOverlayWindow = std::make_shared<DisplayOverlayWindow>("gWindows.DisplayOverlay", "Display Overlay");
    gui->AddGuiWindow(mDisplayOverlayWindow);

    mTimesplitsWindow = std::make_shared<TimesplitsWindow>("gWindows.Timesplits", "Time Splits Window");
    gui->AddGuiWindow(mTimesplitsWindow);

    mTimesplitsSettingsWindow = std::make_shared<TimesplitsSettingsWindow>(
        "gWindows.Timesplits.Settings", "Time Splits Settings Window", ImVec2(567, 97));
    gui->AddGuiWindow(mTimesplitsSettingsWindow);

    mNotificationWindow = std::make_shared<Notification::Window>("gWindows.Notifications", "Notifications Window");
    gui->AddGuiWindow(mNotificationWindow);
    mNotificationWindow->Show();

    mRandoCheckTrackerWindow = std::make_shared<Rando::CheckTracker::CheckTrackerWindow>(
        "gWindows.CheckTracker", "Check Tracker", ImVec2(375, 460));
    gui->AddGuiWindow(mRandoCheckTrackerWindow);

    mRandoCheckTrackerSettingsWindow = std::make_shared<Rando::CheckTracker::SettingsWindow>(
        "gWindows.CheckTrackerSettings", "Check Tracker Settings");
    gui->AddGuiWindow(mRandoCheckTrackerSettingsWindow);

    mInputViewer = std::make_shared<InputViewer>("gWindows.InputViewer", "Input Viewer");
    gui->AddGuiWindow(mInputViewer);
    mInputViewerSettings = std::make_shared<InputViewerSettingsWindow>("gWindows.InputViewerSettings",
                                                                       "Input Viewer Settings", ImVec2(500, 525));
    gui->AddGuiWindow(mInputViewerSettings);
}

void Destroy() {
    auto gui = Ship::Context::GetRawInstance()->GetWindow()->GetGui();

    gui->RemoveAllGuiWindows();
    mBenMenuBar = nullptr;
    mBenMenu = nullptr;
    mModalWindow = nullptr;
    mStatsWindow = nullptr;
    mConsoleWindow = nullptr;
    mGfxDebuggerWindow = nullptr;
    mInputEditorWindow = nullptr;
    mCollisionViewerWindow = nullptr;
    mEventLogWindow = nullptr;
    mNotificationWindow = nullptr;
    mRandoCheckTrackerWindow = nullptr;
    mRandoCheckTrackerSettingsWindow = nullptr;

    mHookDebuggerWindow = nullptr;
    mSaveEditorWindow = nullptr;
    mHudEditorWindow = nullptr;
    mCosmeticEditorWindow = nullptr;
    mActorViewerWindow = nullptr;
    mDLViewerWindow = nullptr;
    mMessageViewerWindow = nullptr;
    mAudioEditorWindow = nullptr;
    mModMenuWindow = nullptr;
    mItemTrackerWindow = nullptr;
    mItemTrackerSettingsWindow = nullptr;
    mInputViewer = nullptr;
    mInputViewerSettings = nullptr;
}

void RegisterPopup(std::string title, std::string message, std::string button1, std::string button2,
                   std::function<void()> button1callback, std::function<void()> button2callback) {
    mModalWindow->RegisterPopup(title, message, button1, button2, button1callback, button2callback);
}

size_t PopupsQueued() {
    return mModalWindow->PopupsQueued();
}

bool DismissPopup(std::string title) {
    if (mModalWindow->IsPopupOpen(title)) {
        mModalWindow->DismissPopup();
        return true;
    }
    return false;
}

void SetDisplayOverlayVisibility(bool visible) {
    if (mDisplayOverlayWindow != nullptr) {
        if (visible) {
            mDisplayOverlayWindow->Show();
        } else {
            mDisplayOverlayWindow->Hide();
        }
    } else {
        CVarSetInteger("gWindows.DisplayOverlay", visible ? 1 : 0);
    }
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
}

} // namespace BenGui
