#include "TimeSplits.h"
#include "soh/UIWidgets.hpp"
#include "soh/Enhancements/randomizer/3drando/item_list.hpp"
#include "soh/Enhancements/gameplaystats.h"
#include "soh/SaveManager.h"
#include "soh/util.h"
#include <vector>

#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

extern "C" {
    extern SaveContext gSaveContext;
}


// ImVec4 Colors
#define COLOR_WHITE ImVec4(1.00f, 1.00f, 1.00f, 1.00f)
#define COLOR_RED ImVec4(1.00f, 0.00f, 0.00f, 1.00f)
#define COLOR_GREEN ImVec4(0.10f, 1.00f, 0.10f, 1.00f)
#define COLOR_BLUE ImVec4(0.00f, 0.33f, 1.00f, 1.00f)
#define COLOR_PURPLE ImVec4(0.54f, 0.19f, 0.89f, 1.00f)
#define COLOR_YELLOW ImVec4(1.00f, 1.00f, 0.00f, 1.00f)
#define COLOR_ORANGE ImVec4(1.00f, 0.67f, 0.11f, 1.00f)
#define COLOR_LIGHT_BLUE ImVec4(0.00f, 0.88f, 1.00f, 1.00f)
#define COLOR_GREY ImVec4(0.78f, 0.78f, 0.78f, 1.00f)

static std::vector<int> splitItem;
static std::vector<std::string> splitTime;
static std::vector<int> splitStatus;
static uint32_t itemReference = ITEM_STICK;
static std::string status = "";
static ImVec4 statusColor = COLOR_WHITE;
std::time_t convertedTime;
const char* itemTime;

std::string formatTimestampTimeSplit(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

void TimeSplitTimeHandler() {
    uint32_t num = GAMEPLAYSTAT_TOTAL_TIME;
    std::string temp = formatTimestampTimeSplit(num);
    itemTime = temp.c_str();
}

void TimeSplitSplitsHandler(GetItemEntry itemEntry) {
    if (itemEntry.modIndex != 0) {
        return;
    }
    uint32_t loopCounter = 0;
    for (auto& str : splitItem) {
        uint32_t num = GAMEPLAYSTAT_TOTAL_TIME;
        std::string temp = formatTimestampTimeSplit(num);
        const char* timeValue = temp.c_str();

        if (itemEntry.itemId == splitItem[loopCounter]) {
            splitTime[loopCounter] = timeValue;
            splitStatus[loopCounter] = 1;
        }
        loopCounter++;
    }
}

void DrawTimeSplitSplits(){
    uint32_t loopCounter = 0;

    ImGui::BeginTable("Splits", 4);
    ImGui::TableNextColumn();
    ImGui::TextColored(COLOR_LIGHT_BLUE, "Items");
    ImGui::TableNextColumn();
    ImGui::TextColored(COLOR_LIGHT_BLUE, "Current Times");
    ImGui::TableNextColumn();
    ImGui::TextColored(COLOR_LIGHT_BLUE, "+/-");
    ImGui::TableNextColumn();
    ImGui::TextColored(COLOR_LIGHT_BLUE, "Previous Best");

    for (auto& str : splitItem) {
        ImGui::TableNextColumn();
        ImGui::TextColored(COLOR_WHITE, SohUtils::itemNames[splitItem[loopCounter]].c_str());
        ImGui::TableNextColumn();

        if (splitStatus[loopCounter] == 0) {
            uint32_t num = GAMEPLAYSTAT_TOTAL_TIME;
            std::string temp = formatTimestampTimeSplit(num);
            itemTime = temp.c_str();
            //TimeSplitTimeHandler();
            ImGui::TextColored(COLOR_WHITE, itemTime);
        } else {
            ImGui::TextColored(COLOR_GREEN, splitTime[loopCounter].c_str());
        }
        ImGui::TableNextColumn();
        // to-do: split difference +/- with colored indicators

        ImGui::TextColored(COLOR_WHITE, "Not Supported");
        ImGui::TableNextColumn();
        ImGui::Text("0:00:00.0");

        loopCounter++;
    }
    ImGui::EndTable();
}

void DrawTimeSplitOptions() {
    if (ImGui::BeginCombo("Item List", SohUtils::itemNames[itemReference].c_str())) {
        for (int i = 0; i < SohUtils::itemNames.size(); i++) {
            if (ImGui::Selectable(SohUtils::itemNames[i].c_str())) {
                itemReference = i;
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button("Add Item")) {
        
        splitItem.push_back(itemReference);
        splitTime.push_back("");
        splitStatus.push_back(0);
        std::string itemEntry = SohUtils::itemNames[itemReference];
        status = itemEntry + std::string(" Added to List");
        statusColor = COLOR_GREEN;
    }
    ImGui::SameLine(0);
    if (ImGui::Button("Reset List")) {
        splitItem.clear();
        splitTime.clear();
        splitStatus.clear();
        status = "List has been reset";
        statusColor = COLOR_RED;
    }
    ImGui::SameLine(0);
    if (ImGui::Button("Load List")) {
    
    }
    ImGui::TextColored(statusColor, status.c_str());
    UIWidgets::PaddedSeparator();
}

void TimeSplitWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Time Splitter Window", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }
    if (ImGui::CollapsingHeader("Time Splitter")) {
        DrawTimeSplitOptions();
    }
    DrawTimeSplitSplits();
    ImGui::End();
}

void TimeSplitWindow::InitElement() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
            TimeSplitSplitsHandler(itemEntry); 
    });
}