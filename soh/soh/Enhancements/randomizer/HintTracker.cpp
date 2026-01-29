#include "HintTracker.h"

#include "libultraship/controller/controldeck/ControlDeck.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/randomizer_entrance_tracker.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"

extern "C" {
#include "variables.h"
extern PlayState* gPlayState;
}

namespace HintTracker {
Color_RGBA8 Color_Background = { 0, 0, 0, 255 };
static ImGuiTextFilter hintSearch;
static WidgetInfo backgroundColorWidget;
static WidgetInfo windowTypeWidget;

void RegisterHintTrackerWidgets() {
    backgroundColorWidget = { .name = "Background Color##HintTracker", .type = WidgetType::WIDGET_CVAR_COLOR_PICKER };
    backgroundColorWidget.CVar(CVAR_TRACKER_HINT("BgColor"))
        .Options(UIWidgets::ColorPickerOptions()
                     .Color(THEME_COLOR)
                     .DefaultValue(Color_Bg_Default)
                     .UseAlpha()
                     .ShowReset()
                     .ShowRandom());
    SohGui::GetSohMenu()->AddSearchWidget({ backgroundColorWidget, "Randomizer", "Hint Tracker", "General Settings" });

    windowTypeWidget = { .name = "Window Type##HintTracker", .type = WidgetType::WIDGET_CVAR_COMBOBOX };
    windowTypeWidget.CVar(CVAR_TRACKER_HINT("WindowType"))
        .Options(
            UIWidgets::ComboboxOptions().DefaultIndex(TRACKER_WINDOW_WINDOW).Color(THEME_COLOR).ComboMap(windowType));
    SohGui::GetSohMenu()->AddSearchWidget({ windowTypeWidget, "Randomizer", "Hint Tracker", "General Settings" });
}

void HintTrackerSettingsWindow::DrawElement() {
    SohGui::GetSohMenu()->MenuDrawItem(backgroundColorWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);
    SohGui::GetSohMenu()->MenuDrawItem(windowTypeWidget, ImGui::GetContentRegionAvail().x, THEME_COLOR);
    if (CVarGetInteger(CVAR_TRACKER_HINT("WindowType"), TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        UIWidgets::CVarCheckbox("Enable Dragging", CVAR_TRACKER_HINT("Draggable"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR).DefaultValue(true));
        UIWidgets::CVarCheckbox("Only Enable While Paused", CVAR_TRACKER_HINT("ShowOnlyPaused"),
                                UIWidgets::CheckboxOptions().Color(THEME_COLOR));
        UIWidgets::CVarCombobox("Display mode", CVAR_TRACKER_HINT("DisplayType"), showMode,
                                UIWidgets::ComboboxOptions().Color(THEME_COLOR).DefaultIndex(0));
        if (CVarGetInteger(CVAR_TRACKER_HINT("DisplayType"), TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_COMBO_BUTTON) {
            UIWidgets::CVarBtnSelector("Button Combo", CVAR_TRACKER_HINT("DisplayBtn"),
                                       UIWidgets::BtnSelectorOptions().Color(THEME_COLOR).DefaultValue(BTN_L | BTN_R));
        }
    }
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

void DrawHint(const char* itemName, const char* locationName, ImGuiTextFilter& filter) {
    std::string searchable = std::string(itemName) + " " + std::string(locationName);
    if (filter.IsActive() && !filter.PassFilter(searchable.c_str())) {
        return;
    }
    ImGui::BulletText("%s - %s", itemName, locationName);
}

void DrawHint(const char* hintText, ImGuiTextFilter& filter) {
    if (filter.IsActive() && !filter.PassFilter(hintText)) {
        return;
    }
    ImGui::BulletText("%s", hintText);
}

void HintTrackerWindow::DrawElement() {
    if (CVarGetInteger(CVAR_TRACKER_HINT("WindowType"), TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        if (CVarGetInteger(CVAR_TRACKER_HINT("ShowOnlyPaused"), 0) &&
            (gPlayState == nullptr || gPlayState->pauseCtx.state == 0)) {
            return;
        }
        if (CVarGetInteger(CVAR_TRACKER_HINT("DisplayType"), TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_COMBO_BUTTON) {
            const int32_t packed = CVarGetInteger(CVAR_TRACKER_HINT("DisplayBtn"), BTN_L | BTN_R);
            uint16_t mask = static_cast<uint16_t>(packed & 0xFFFF);
            if (!(mask != 0 && CHECK_BTN_ALL(gGameState->input[0].cur.button, mask))) {
                return;
            }
        }
    }
    Color_Background = CVarGetColor(CVAR_TRACKER_HINT("BgColor.Value"), Color_Bg_Default);
    if (Trackers::BeginFloatWindows(
            "Hint Tracker", mIsVisible, Color_Background,
            static_cast<TrackerWindowType>(CVarGetInteger(CVAR_TRACKER_HINT("WindowType"), TRACKER_WINDOW_WINDOW)),
            CVarGetInteger(CVAR_TRACKER_HINT("Draggable"), 1), ImGuiWindowFlags_NoScrollbar)) {
        if (!GameInteractor::IsSaveLoaded() || mHintTable == nullptr) {
            ImGui::Text("Waiting for file load..."); // TODO Language
            Trackers::EndFloatWindows();
            return;
        }
        ImGui::BeginChild("HintData");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 130);
        UIWidgets::PushStyleInput(THEME_COLOR);
        hintSearch.Draw();
        UIWidgets::PopStyleInput();
        ImGui::Spacing();
        if (UIWidgets::Button(
                "Expand All",
                UIWidgets::ButtonOptions().Color(THEME_COLOR).Size({ ImGui::GetContentRegionAvail().x / 2 - 6, 0 }))) {
            mDoCollapseOrExpand = true;
            mExpand = true;
        }
        ImGui::SameLine();
        if (UIWidgets::Button(
                "Collapse All",
                UIWidgets::ButtonOptions().Color(THEME_COLOR).Size({ ImGui::GetContentRegionAvail().x - 6, 0 }))) {
            mDoCollapseOrExpand = true;
            mExpand = false;
        }
        if (RAND_GET_OPTION(RSK_TOT_ALTAR_HINT).Is(RO_GENERIC_ON)) {
            if (mDoCollapseOrExpand) {
                ImGui::SetNextItemOpen(mExpand, ImGuiCond_Always);
            }
            if (ImGui::TreeNode("Altar Hints")) {
                Rando::Hint& childAltarHint = (*mHintTable)[RH_ALTAR_CHILD];
                if (childAltarHint.IsDiscovered()) {
                    DrawHint("Kokiri Emerald", childAltarHint.GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                    DrawHint("Goron Ruby", childAltarHint.GetAreaName(1).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                    DrawHint("Zora Sapphire", childAltarHint.GetAreaName(2).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                }
                Rando::Hint& adultAltarHint = (*mHintTable)[RH_ALTAR_ADULT];
                if (adultAltarHint.IsDiscovered()) {
                    DrawHint("Light Medallion", adultAltarHint.GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                    DrawHint("Forest Medallion", adultAltarHint.GetAreaName(1).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                    DrawHint("Fire Medallion", adultAltarHint.GetAreaName(2).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                    DrawHint("Water Medallion", adultAltarHint.GetAreaName(3).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                    DrawHint("Spirit Medallion", adultAltarHint.GetAreaName(4).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                    DrawHint("Shadow Medallion", adultAltarHint.GetAreaName(5).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                }
                ImGui::TreePop();
            }
        }
        if (mDoCollapseOrExpand) {
            ImGui::SetNextItemOpen(mExpand, ImGuiCond_Always);
        }
        if (ImGui::TreeNode("Area Hints")) {
            for (auto hint : mItemAreaHints) {
                if (hint->IsDiscovered()) {
                    for (size_t i = 0; i < hint->GetNumberOfMessages(); i++) {
                        DrawHint(hint->GetItemName(i).GetForCurrentLanguage(MF_CLEAN).c_str(),
                                 hint->GetAreaName(i).GetForCurrentLanguage(MF_CLEAN).c_str(), hintSearch);
                    }
                }
            }
            ImGui::TreePop();
        }
        if (mDoCollapseOrExpand) {
            ImGui::SetNextItemOpen(mExpand, ImGuiCond_Always);
        }
        if (ImGui::TreeNode("Location Hints")) {
            for (auto hint : mItemLocationHints) {
                if (hint->IsDiscovered()) {
                    for (size_t i = 0; i < hint->GetNumberOfMessages(); i++) {
                        DrawHint(hint->GetItemName(i).GetForCurrentLanguage(MF_CLEAN).c_str(),
                                 Rando::StaticData::GetLocation(hint->GetHintedLocations()[i])->GetName().c_str(),
                                 hintSearch);
                    }
                }
            }
            ImGui::TreePop();
        }
        if (mDoCollapseOrExpand) {
            ImGui::SetNextItemOpen(mExpand, ImGuiCond_Always);
        }
        if (ImGui::TreeNode("Foolish Hints")) {
            for (auto hint : mFoolishHints) {
                if (hint->IsDiscovered()) {
                    for (size_t i = 0; i < hint->GetNumberOfMessages(); i++) {
                        DrawHint(hint->GetAreaName(i).GetForCurrentLanguage(MF_CLEAN).c_str(), hintSearch);
                    }
                }
            }
            ImGui::TreePop();
        }
        if (RAND_GET_OPTION(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS)) {
            if (mDoCollapseOrExpand) {
                ImGui::SetNextItemOpen(mExpand, ImGuiCond_Always);
            }
            if (ImGui::TreeNode("Way of the Hero Hints")) {
                for (auto hint : mWothHints) {
                    if (hint->IsDiscovered()) {
                        for (size_t i = 0; i < hint->GetNumberOfMessages(); i++) {
                            DrawHint(hint->GetAreaName(i).GetForCurrentLanguage(MF_CLEAN).c_str(), hintSearch);
                        }
                    }
                }
                ImGui::TreePop();
            }
        }
        if (RAND_GET_OPTION(RSK_GANONS_TRIALS).IsNot(RO_GANONS_TRIALS_SKIP)) {
            if (mDoCollapseOrExpand) {
                ImGui::SetNextItemOpen(mExpand, ImGuiCond_Always);
            }
            if (ImGui::TreeNode("Trial Hints")) {
                for (auto hint : mTrialHints) {
                    if (hint->IsDiscovered()) {
                        for (size_t i = 0; i < hint->GetHintedTrials().size(); i++) {
                            DrawHint(
                                OTRGlobals::Instance->gRandoContext->GetTrial(hint->GetHintedTrials()[i])
                                    ->GetName()
                                    .GetForCurrentLanguage(MF_CLEAN)
                                    .c_str(),
                                OTRGlobals::Instance->gRandoContext->GetTrial(hint->GetHintedTrials()[i])->IsRequired()
                                    ? "Required"
                                    : "Skipped",
                                hintSearch);
                        }
                    }
                }
                ImGui::TreePop();
            }
        }
        if (RAND_GET_OPTION(RSK_SHUFFLE_WARP_SONGS).Is(RO_GENERIC_ON) &&
            RAND_GET_OPTION(RSK_WARP_SONG_HINTS).Is(RO_GENERIC_ON)) {
            if (mDoCollapseOrExpand) {
                ImGui::SetNextItemOpen(mExpand, ImGuiCond_Always);
            }
            if (ImGui::TreeNode("Warp Song Hints")) {
                if ((*mHintTable)[RH_MINUET_WARP_LOC].IsDiscovered()) {
                    DrawHint("Minuet of Forest",
                             (*mHintTable)[RH_MINUET_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                }
                if ((*mHintTable)[RH_BOLERO_WARP_LOC].IsDiscovered()) {
                    DrawHint("Bolero of Fire",
                             (*mHintTable)[RH_BOLERO_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                }
                if ((*mHintTable)[RH_SERENADE_WARP_LOC].IsDiscovered()) {
                    DrawHint("Serenade of Water",
                             (*mHintTable)[RH_SERENADE_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                }
                if ((*mHintTable)[RH_REQUIEM_WARP_LOC].IsDiscovered()) {
                    DrawHint("Requiem of Spirit",
                             (*mHintTable)[RH_REQUIEM_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                }
                if ((*mHintTable)[RH_NOCTURNE_WARP_LOC].IsDiscovered()) {
                    DrawHint("Nocturne of Shadow",
                             (*mHintTable)[RH_NOCTURNE_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                }
                if ((*mHintTable)[RH_PRELUDE_WARP_LOC].IsDiscovered()) {
                    DrawHint("Prelude of Light",
                             (*mHintTable)[RH_PRELUDE_WARP_LOC].GetAreaName(0).GetForCurrentLanguage(MF_CLEAN).c_str(),
                             hintSearch);
                }
                ImGui::TreePop();
            }
        }
        if (mDoCollapseOrExpand) {
            mDoCollapseOrExpand = false;
        }
        ImGui::EndChild();

        Trackers::EndFloatWindows();
    }
}

static RegisterMenuInitFunc menuInitFunc(RegisterHintTrackerWidgets);
} // namespace HintTracker
