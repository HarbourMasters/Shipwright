#include "HintTracker.h"

#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/static_data.h"
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
    mItemAreaHints.clear();
    mItemLocationHints.clear();
    mFoolishHints.clear();
    mWothHints.clear();
    mTrialHints.clear();

    for (auto& hint : *mHintTable) {
        if (hint.IsEnabled() && hint.GetDistribution() != "Junk") {
            if (hint.GetKey() >= RH_MINUET_WARP_LOC && hint.GetKey() <= RH_PRELUDE_WARP_LOC) {
                continue;
            } else if (hint.GetHintType() == HINT_TYPE_AREA || hint.GetHintType() == HINT_TYPE_ITEM_AREA) {
                mItemAreaHints.push_back(&hint);
            } else if (hint.GetHintType() == HINT_TYPE_ITEM) {
                mItemLocationHints.push_back(&hint);
            } else if (hint.GetHintType() == HINT_TYPE_FOOLISH) {
                mFoolishHints.push_back(&hint);
            } else if (hint.GetHintType() == HINT_TYPE_WOTH) {
                mWothHints.push_back(&hint);
            } else if (hint.GetHintType() == HINT_TYPE_TRIAL) {
                mTrialHints.push_back(&hint);
            }
        }
    }
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
    ImGui::BeginChild("HintData");
    if (RAND_GET_OPTION(RSK_TOT_ALTAR_HINT).Is(RO_GENERIC_ON)) {
        if (ImGui::TreeNode("Altar Hints")) {
            Rando::Hint& childAltarHint = (*mHintTable)[RH_ALTAR_CHILD];
            if (childAltarHint.IsDiscovered()) {
                ImGui::BulletText("Kokiri Emerald - %s",
                                  childAltarHint.GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str());
                ImGui::BulletText("Goron Ruby - %s",
                                  childAltarHint.GetAreaName(1).GetForCurrentLanguage(MF_CLEAN).c_str());
                ImGui::BulletText("Zora Sapphire - %s",
                                  childAltarHint.GetAreaName(2).GetForCurrentLanguage(MF_CLEAN).c_str());
            }
            Rando::Hint& adultAltarHint = (*mHintTable)[RH_ALTAR_ADULT];
            if (adultAltarHint.IsDiscovered()) {
                ImGui::BulletText("Light Medallion - %s",
                                  adultAltarHint.GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str());
                ImGui::BulletText("Forest Medallion - %s",
                                  adultAltarHint.GetAreaName(1).GetForCurrentLanguage(MF_CLEAN).c_str());
                ImGui::BulletText("Fire Medallion - %s",
                                  adultAltarHint.GetAreaName(2).GetForCurrentLanguage(MF_CLEAN).c_str());
                ImGui::BulletText("Water Medallion - %s",
                                  adultAltarHint.GetAreaName(3).GetForCurrentLanguage(MF_CLEAN).c_str());
                ImGui::BulletText("Spirit Medallion - %s",
                                  adultAltarHint.GetAreaName(4).GetForCurrentLanguage(MF_CLEAN).c_str());
                ImGui::BulletText("Shadow Medallion - %s",
                                  adultAltarHint.GetAreaName(5).GetForCurrentLanguage(MF_CLEAN).c_str());
            }
            ImGui::TreePop();
        }
    }
    if (ImGui::TreeNode("Area Hints")) {
        for (auto hint : mItemAreaHints) {
            if (hint->IsDiscovered()) {
                for (size_t i = 0; i < hint->GetNumberOfMessages(); i++) {
                    ImGui::BulletText("%s - %s", hint->GetItemName(i).GetForCurrentLanguage(MF_CLEAN).c_str(),
                                      hint->GetAreaName(i).GetForCurrentLanguage(MF_CLEAN).c_str());
                }
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Location Hints")) {
        for (auto hint : mItemLocationHints) {
            if (hint->IsDiscovered()) {
                for (size_t i = 0; i < hint->GetNumberOfMessages(); i++) {
                    ImGui::BulletText("%s - %s", hint->GetItemName(i).GetForCurrentLanguage(MF_CLEAN).c_str(),
                                      Rando::StaticData::GetLocation(hint->GetHintedLocations()[i])->GetName().c_str());
                }
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Foolish Hints")) {
        for (auto hint : mFoolishHints) {
            if (hint->IsDiscovered()) {
                for (size_t i = 0; i < hint->GetNumberOfMessages(); i++) {
                    ImGui::BulletText("%s", hint->GetAreaName(i).GetForCurrentLanguage(MF_CLEAN).c_str());
                }
            }
        }
        ImGui::TreePop();
    }
    if (RAND_GET_OPTION(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS)) {
        if (ImGui::TreeNode("Way of the Hero Hints")) {
            for (auto hint : mWothHints) {
                if (hint->IsDiscovered()) {
                    for (size_t i = 0; i < hint->GetNumberOfMessages(); i++) {
                        ImGui::BulletText("%s", hint->GetAreaName(i).GetForCurrentLanguage(MF_CLEAN).c_str());
                    }
                }
            }
            ImGui::TreePop();
        }
    }
    if (RAND_GET_OPTION(RSK_GANONS_TRIALS).IsNot(RO_GANONS_TRIALS_SKIP)) {
        if (ImGui::TreeNode("Trial Hints")) {
            for (auto hint : mTrialHints) {
                if (hint->IsDiscovered()) {
                    for (size_t i = 0; i < hint->GetHintedTrials().size(); i++) {
                        ImGui::BulletText(
                            "%s - %s",
                            OTRGlobals::Instance->gRandoContext->GetTrial(hint->GetHintedTrials()[i])
                                ->GetName()
                                .GetForCurrentLanguage(MF_CLEAN)
                                .c_str(),
                            OTRGlobals::Instance->gRandoContext->GetTrial(hint->GetHintedTrials()[i])->IsRequired()
                                ? "Required"
                                : "Skipped");
                    }
                }
            }
            ImGui::TreePop();
        }
    }
    if (RAND_GET_OPTION(RSK_SHUFFLE_WARP_SONGS).Is(RO_GENERIC_ON) &&
        RAND_GET_OPTION(RSK_WARP_SONG_HINTS).Is(RO_GENERIC_ON)) {
        if (ImGui::TreeNode("Warp Song Hints")) {
            if ((*mHintTable)[RH_MINUET_WARP_LOC].IsDiscovered()) {
                ImGui::BulletText(
                    "Minuet of Forest - %s",
                    (*mHintTable)[RH_MINUET_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str());
            }
            if ((*mHintTable)[RH_BOLERO_WARP_LOC].IsDiscovered()) {
                ImGui::BulletText(
                    "Bolero of Fire - %s",
                    (*mHintTable)[RH_BOLERO_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str());
            }
            if ((*mHintTable)[RH_SERENADE_WARP_LOC].IsDiscovered()) {
                ImGui::BulletText(
                    "Serenade of Water - %s",
                    (*mHintTable)[RH_SERENADE_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str());
            }
            if ((*mHintTable)[RH_REQUIEM_WARP_LOC].IsDiscovered()) {
                ImGui::BulletText(
                    "Requiem of Spirit - %s",
                    (*mHintTable)[RH_REQUIEM_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str());
            }
            if ((*mHintTable)[RH_NOCTURNE_WARP_LOC].IsDiscovered()) {
                ImGui::BulletText(
                    "Nocturne of Shadow - %s",
                    (*mHintTable)[RH_NOCTURNE_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str());
            }
            if ((*mHintTable)[RH_PRELUDE_WARP_LOC].IsDiscovered()) {
                ImGui::BulletText(
                    "Prelude of Light - %s",
                    (*mHintTable)[RH_PRELUDE_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str());
            }
            ImGui::TreePop();
        }
    }
    ImGui::EndChild();

    EndFloatWindows();
}
} // namespace HintTracker
