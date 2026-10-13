
#include "Timesplits.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
#include "2s2h/BenGui/UIWidgets.hpp"

extern "C" {
#include "variables.h"
uint64_t GetUnixTimestamp();
}

#include "ShipUtils.h"
#include "interface/parameter_static/parameter_static.h"
#include "assets/archives/icon_item_static/icon_item_static_yar.h"
#include "GameInteractor/GameInteractor.h"

#include <fast/Fast3dGui.h>

#define BLANK_SPLIT "--:--:--.-"

// ImVec4 Colors
#define COLOR_WHITE ImVec4(1.00f, 1.00f, 1.00f, 1.00f)
#define COLOR_GREY ImVec4(0.78f, 0.78f, 0.78f, 1.00f)
#define COLOR_GREEN ImVec4(0.10f, 1.00f, 0.10f, 1.00f)
#define COLOR_RED ImVec4(1.00f, 0.00f, 0.00f, 1.00f)
#define COLOR_LIGHTGREEN ImVec4(0.42f, 0.86f, 0.38f, 1.0f)
#define COLOR_LIGHTRED ImVec4(0.87f, 0.40f, 0.40f, 1.0f)

std::vector<TimesplitObject> splitList;
std::vector<TimesplitObject> comparisonList;
ImGuiTableFlags tableColumnFlags = ImGuiTableColumnFlags_None;
ImVec4 splitOpacity = { 0, 0, 0, 0.5f };

uint32_t GetTotalTime() {
    return ((GetUnixTimestamp() - gSaveContext.save.shipSaveInfo.fileCreatedAt) / 100);
}

SplitTextObject GetCurrentTimeTextDisplay(TimesplitObject split) {
    SplitTextObject textDisplay;

    switch (split.splitStatus) {
        case SPLIT_INACTIVE:
        case SPLIT_SKIPPED:
            textDisplay.timeDisplay = 0;
            textDisplay.colorDisplay = COLOR_GREY;
            return textDisplay;
        case SPLIT_ACTIVE:
            textDisplay.timeDisplay = GetTotalTime();
            textDisplay.colorDisplay = COLOR_WHITE;
            return textDisplay;
        case SPLIT_COMPLETE:
            textDisplay.timeDisplay = split.splitCurrentTime;
            if (split.splitCurrentTime > split.splitPreviousBest) {
                textDisplay.colorDisplay = COLOR_RED;
            } else if (split.splitCurrentTime == split.splitPreviousBest) {
                textDisplay.colorDisplay = COLOR_WHITE;
            } else if (split.splitCurrentTime < split.splitPreviousBest) {
                textDisplay.colorDisplay = COLOR_GREEN;
            }
            if (split.splitPreviousBest == 0) {
                textDisplay.colorDisplay = COLOR_WHITE;
            }
            return textDisplay;
        default:
            break;
    }
}

SplitTextObject GetComparisonTimeTextDisplay(TimesplitObject split, TimesplitObject splitCompare) {
    SplitTextObject textDisplay;

    textDisplay.timeDisplay = splitCompare.splitPreviousBest;
    if (splitCompare.splitPreviousBest == 0) {
        textDisplay.timeDisplay = splitCompare.splitPreviousBest;
        textDisplay.colorDisplay = COLOR_GREY;
    } else {
        if (split.splitStatus == SPLIT_ACTIVE) {
            if (GetTotalTime() <= splitCompare.splitPreviousBest) {
                textDisplay.colorDisplay = COLOR_LIGHTGREEN;
            } else {
                textDisplay.colorDisplay = COLOR_LIGHTRED;
            }
        } else {
            if (split.splitCurrentTime <= splitCompare.splitPreviousBest) {
                textDisplay.colorDisplay = COLOR_LIGHTRED;
            } else {
                textDisplay.colorDisplay = COLOR_LIGHTGREEN;
            }
        }
    }

    return textDisplay;
}

SplitTextObject GetTimeDiffTextDisplay(TimesplitObject split) {
    SplitTextObject textDisplay;

    switch (split.splitStatus) {
        case SPLIT_INACTIVE:
        case SPLIT_SKIPPED:
            textDisplay.timeDisplay = split.splitPreviousBest;
            textDisplay.colorDisplay = COLOR_GREY;
            return textDisplay;
        case SPLIT_ACTIVE:
            if (GetTotalTime() > split.splitPreviousBest) {
                textDisplay.timeDisplay = GetTotalTime() - split.splitPreviousBest;
                textDisplay.colorDisplay = COLOR_RED;
            } else if (GetTotalTime() == split.splitPreviousBest) {
                textDisplay.timeDisplay = GetTotalTime();
                textDisplay.colorDisplay = COLOR_WHITE;
            } else if (GetTotalTime() < split.splitPreviousBest) {
                textDisplay.timeDisplay = split.splitPreviousBest - GetTotalTime();
                textDisplay.colorDisplay = COLOR_GREEN;
            }
            if (split.splitPreviousBest == 0) {
                textDisplay.colorDisplay = COLOR_WHITE;
            }
            return textDisplay;
        case SPLIT_COMPLETE:
            if (split.splitCurrentTime > split.splitPreviousBest) {
                textDisplay.timeDisplay = split.splitCurrentTime - split.splitPreviousBest;
                textDisplay.colorDisplay = COLOR_RED;
            } else if (split.splitCurrentTime == split.splitPreviousBest) {
                textDisplay.timeDisplay = split.splitCurrentTime;
                textDisplay.colorDisplay = COLOR_WHITE;
            } else if (split.splitCurrentTime < split.splitPreviousBest) {
                textDisplay.timeDisplay = split.splitPreviousBest - split.splitCurrentTime;
                textDisplay.colorDisplay = COLOR_GREEN;
            }
            if (split.splitPreviousBest == 0) {
                textDisplay.colorDisplay = COLOR_WHITE;
            }
            return textDisplay;
        default:
            break;
    }
}

void TableCellCenteredText(ImVec4 color, const char* text) {
    float textHeight = ImGui::GetTextLineHeight();
    float offsetY = (32.0f - textHeight + 10.0f) * 0.5f;
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
    ImGui::TextColored(color, "%s", text);
}

void SplitsPushImageButtonStyle() {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
}

void SplitsPopImageButtonStyle() {
    ImGui::PopStyleColor(3);
}

void DrawSplitsList(bool isMain) {
    float columnSizeMultiplier = isMain ? 1.0f : 1.5f;
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 7.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10, 0));

    if (isMain) {
        ImGui::Begin("Timesplits", nullptr,
                     ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDocking |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollWithMouse |
                         ImGuiWindowFlags_NoScrollbar);
    }

    if (ImGui::BeginChild("SplitChild", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar)) {
        if (ImGui::BeginTable("Splits", 5,
                              ImGuiTableFlags_ScrollY | ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable)) {
            ImGui::TableSetupColumn("Item Image",
                                    ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel,
                                    (28.0f * columnSizeMultiplier));
            ImGui::TableSetupColumn("Item Name");
            ImGui::TableSetupColumn("Current Time");
            ImGui::TableSetupColumn("+/-");
            ImGui::TableSetupColumn("Previous Best ");
            if (tableColumnFlags != ImGuiTableColumnFlags_None) {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableHeadersRow();
            }

            for (size_t i = 0; i < splitList.size(); i++) {
                ImGui::PushID(i);

                // Item Image Column
                ImGui::TableNextColumn();

                if (CVarGetInteger("gSettings.TimeSplits.Highlight", 0) && splitList[i].splitStatus == SPLIT_ACTIVE) {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(47, 79, 90, 255));
                }

                SplitsPushImageButtonStyle();
                if (ImGui::ImageButton(
                        std::to_string(i).c_str(),
                        std::dynamic_pointer_cast<Fast::Fast3dGui>(
                            Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                            ->GetTextureByName(splitList[i].splitType == SPLIT_TYPE_NORMAL
                                                   ? GetItemImageById(splitList[i].splitId)
                                                   : gPauseUnusedCursorTex),
                        splitList[i].splitType == SPLIT_TYPE_NORMAL ? GetItemImageSizeById(splitList[i].splitId)
                                                                    : ImVec2(32.0f, 32.0f),
                        ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0),
                        splitList[i].splitType == SPLIT_TYPE_NORMAL ? Ship_GetItemColorTint(splitList[i].splitId)
                                                                    : ImVec4(1, 1, 1, 1))) {
                    SkipSplitEntry(i);
                };
                SplitsPopImageButtonStyle();

                // Item Name Column
                ImGui::TableNextColumn();
                TableCellCenteredText(COLOR_WHITE, splitList[i].splitName.c_str());

                // Current Time Column
                ImGui::TableNextColumn();
                TableCellCenteredText(
                    GetCurrentTimeTextDisplay(splitList[i]).colorDisplay,
                    !gPlayState ? BLANK_SPLIT
                                : Ship_FormatTimeDisplay(GetCurrentTimeTextDisplay(splitList[i]).timeDisplay).c_str());

                // +/- Column
                ImGui::TableNextColumn();
                TableCellCenteredText(
                    GetTimeDiffTextDisplay(splitList[i]).colorDisplay,
                    !gPlayState ? BLANK_SPLIT
                                : Ship_FormatTimeDisplay(GetTimeDiffTextDisplay(splitList[i]).timeDisplay).c_str());
                if (CVarGetInteger("gSettings.TimeSplits.Compare", 0) && comparisonList.size() != 0) {
                    !gPlayState ? ImGui::TextColored(COLOR_WHITE, BLANK_SPLIT)
                    : i < comparisonList.size()
                        ? ImGui::TextColored(
                              GetComparisonTimeTextDisplay(splitList[i], comparisonList[i]).colorDisplay, "%s",
                              Ship_FormatTimeDisplay(
                                  GetComparisonTimeTextDisplay(splitList[i], comparisonList[i]).timeDisplay)
                                  .c_str())
                        : ImGui::TextColored(COLOR_GREY, "No Data");
                }

                // Previous Best Column
                ImGui::TableNextColumn();
                TableCellCenteredText(COLOR_WHITE, Ship_FormatTimeDisplay(splitList[i].splitPreviousBest).c_str());
                if (CVarGetInteger("gSettings.TimeSplits.Compare", 0) && comparisonList.size() != 0) {
                    ImGui::TextColored(COLOR_GREY, "%s",
                                       i < comparisonList.size()
                                           ? Ship_FormatTimeDisplay(comparisonList[i].splitPreviousBest).c_str()
                                           : "No Data");
                }

                ImGui::PopID();

                if (CVarGetInteger("gSettings.TimeSplits.Follow", 0) && splitList[i].splitStatus == SPLIT_ACTIVE) {
                    ImGui::SetScrollHereY();
                }
            }

            ImGui::EndTable();
        }
    }
    ImGui::EndChild();

    if (isMain) {
        ImGui::End();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(1);
}

void UpdateSplitSettings(uint32_t settingName) {
    switch (settingName) {
        case SPLIT_HEADERS:
            tableColumnFlags = CVarGetInteger("gSettings.TimeSplits.ShowHeaders", 0)
                                   ? ImGuiTableColumnFlags_NoHeaderLabel
                                   : ImGuiTableColumnFlags_None;
            break;
        case SPLIT_OPACITY:
            splitOpacity.w = CVarGetInteger("gSettings.TimeSplits.Opacity", 0) ? 0 : 0.5f;
            break;
        case SPLIT_COMPARE:
            if (CVarGetInteger("gSettings.TimeSplits.Compare", 0)) {
                SplitSaveFileAction(SPLIT_RETRIEVE, "");
                if (savedLists.size() != 0) {
                    SplitLoadComparisonList();
                }
            }
            break;
        default:
            break;
    }
}

void TimesplitsWindow::Draw() {
    if (!CVarGetInteger("gSettings.TimeSplits.Enable", 0)) {
        return;
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, splitOpacity);
    DrawSplitsList(true);
    ImGui::PopStyleColor();
}

void TimesplitsWindow::InitElement() {
    UpdateSplitSettings(SPLIT_HEADERS);
    UpdateSplitSettings(SPLIT_OPACITY);
    UpdateSplitSettings(SPLIT_COMPARE);
}

// void TimesplitsWindow::DrawElement() {
// }
