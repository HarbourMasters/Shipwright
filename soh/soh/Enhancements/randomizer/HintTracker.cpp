#include "HintTracker.h"

#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/SohGui/UIWidgets.hpp"

extern "C" {
#include "variables.h"
}

namespace HintTracker {
Color_RGBA8 Color_Background = { 0, 0, 0, 255 };

// Windowing stuff
void BeginFloatWindows(std::string UniqueName, bool& open, ImGuiWindowFlags flags) {
    ImGuiWindowFlags windowFlags = flags;

    if (windowFlags == 0) {
        windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoFocusOnAppearing;
    }

    if (CVarGetInteger(CVAR_TRACKER_CHECK("WindowType"), TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

        if (!CVarGetInteger(CVAR_TRACKER_CHECK("Draggable"), 1)) {
            windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }
    }
    auto maybeParent = ImGui::GetCurrentWindow();
    ImGuiWindow* window = ImGui::FindWindowByName(UniqueName.c_str());
    if (window != NULL && window->DockTabIsVisible && window->ParentWindow != NULL &&
        std::string(window->ParentWindow->Name).compare(0, strlen("Main - Deck"), "Main - Deck") == 0) {
        Color_Background.a = 255;
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, VecFromRGBA8(Color_Background));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin(UniqueName.c_str(), &open, windowFlags);
}

void EndFloatWindows() {
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::End();
}

// Hook Handlers
void HintTrackerWindow::LoadHintTable(int32_t fileNum) {
    if (!IS_RANDO) {
        return;
    }
    mHintTable = &OTRGlobals::Instance->gRandoContext->hintTable;
}

void HintTrackerWindow::Draw() {
    if (!IsVisible()) {
        return;
    }
    DrawElement();
    SyncVisibilityConsoleVariable();
}

void HintTrackerWindow::InitElement() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>(
        [this](int32_t fileNum) { LoadHintTable(fileNum); });
}

void HintTrackerWindow::UpdateElement() {
}

void HintTrackerWindow::DrawElement() {
    BeginFloatWindows("Hint Tracker", mIsVisible, ImGuiWindowFlags_NoScrollbar);
    if (!GameInteractor::IsSaveLoaded() || mHintTable == nullptr) {
        ImGui::Text("Waiting for file load..."); // TODO Language
        EndFloatWindows();
        return;
    }
    for (auto& hint : *mHintTable) {
        if (hint.IsEnabled() && hint.IsDiscovered()) {
            ImGui::BulletText("%s", hint.GetHintMessage(MF_CLEAN).GetForCurrentLanguage(MF_CLEAN).c_str());
        }
    }
    EndFloatWindows();
}
} // namespace HintTracker