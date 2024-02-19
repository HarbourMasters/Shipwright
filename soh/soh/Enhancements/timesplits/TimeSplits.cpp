#include "TimeSplits.h"
#include "soh/UIWidgets.hpp"
#include "soh/Enhancements/randomizer/3drando/item_list.hpp"
#include "soh/Enhancements/gameplaystats.h"
#include "soh/SaveManager.h"
#include "soh/util.h"
#include <vector>
#include "include/z64item.h"

#include "nlohmann-json/include/nlohmann/json.hpp"
#include <fstream>
#include <filesystem>

#include "soh/OTRGlobals.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/debugger/debugSaveEditor.h"

extern "C" {
    extern SaveContext gSaveContext;
}


// ImVec4 Colors
#define COLOR_WHITE ImVec4(1.00f, 1.00f, 1.00f, 1.00f)
#define COLOR_LIGHT_RED ImVec4(1.0f, 0.05f, 0.0f, 1.0f)
#define COLOR_RED ImVec4(1.00f, 0.00f, 0.00f, 1.00f)
#define COLOR_LIGHT_GREEN ImVec4(0.52f, 1.0f, 0.23f, 1.0f)
#define COLOR_GREEN ImVec4(0.10f, 1.00f, 0.10f, 1.00f)
#define COLOR_BLUE ImVec4(0.00f, 0.33f, 1.00f, 1.00f)
#define COLOR_PURPLE ImVec4(0.54f, 0.19f, 0.89f, 1.00f)
#define COLOR_YELLOW ImVec4(1.00f, 1.00f, 0.00f, 1.00f)
#define COLOR_ORANGE ImVec4(1.00f, 0.67f, 0.11f, 1.00f)
#define COLOR_LIGHT_BLUE ImVec4(0.00f, 0.88f, 1.00f, 1.00f)
#define COLOR_GREY ImVec4(0.78f, 0.78f, 0.78f, 1.00f)

static std::vector<uint32_t> splitItem;
static std::vector<uint32_t> splitTime;
static std::vector<uint32_t> splitBest;
static std::vector<uint32_t> splitPreviousBest;
static std::vector<uint32_t> splitStatus;
static uint32_t itemReference = ITEM_STICK;
static std::string status = "";
static ImVec4 statusColor = COLOR_WHITE;
static uint32_t splitCurNum = 0;
std::string splitAttempt = "Attempt #: 0";
static std::string splitNumDisp = "Attempt #: ";
static ImVec4 colorChoice = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
static const char* backgroundColor;

using json = nlohmann::json;

std::string formatTimestampTimeSplit(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

void TimeSplitSplitsHandler(GetItemEntry itemEntry) {
    if (itemEntry.modIndex != 0) {
        return;
    }
    uint32_t loopCounter = 0;
    for (auto& str : splitItem) {
        if (itemEntry.itemId == splitItem[loopCounter]) {
            splitTime[loopCounter] = GAMEPLAYSTAT_TOTAL_TIME;
            splitStatus[loopCounter] = 1;
            if (splitTime[loopCounter] < splitBest[loopCounter]) {
                splitBest[loopCounter] = splitTime[loopCounter];
            }
        }
        loopCounter++;
    }
}

void DrawTimeSplitSplits(){
    uint32_t loopCounter = 0;
    ImGui::TextColored(COLOR_YELLOW, (splitAttempt).c_str());

    ImGui::BeginTable("Splits", 5, ImGuiTableFlags_Hideable | ImGuiTableFlags_Reorderable);
    ImGui::TableSetupColumn("Item Image", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHeaderLabel, 22.0f);
    ImGui::TableSetupColumn("Item Name");
    ImGui::TableSetupColumn("Current Time", ImGuiTableColumnFlags_WidthFixed, 90.0f);
    ImGui::TableSetupColumn("+/-", ImGuiTableColumnFlags_WidthFixed, 80.0f);
    ImGui::TableSetupColumn("Prev. Best", ImGuiTableColumnFlags_WidthFixed, 90.0f);
    ImGui::TableHeadersRow();

    for (auto& str : splitItem) {
        ImGui::TableNextColumn();
        // Item Image
        ImGui::Image(LUS::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(itemImage[splitItem[loopCounter]]), ImVec2(24.0f, 24.0f), ImVec2(0, 0), ImVec2(1, 1));
        ImGui::TableNextColumn();
        // Item Name
        ImGui::Text(SohUtils::itemNames[splitItem[loopCounter]].c_str());
        ImGui::TableNextColumn();
        // Current Time
        if (splitTime[loopCounter] == 0) {
            ImGui::Text(formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str());
        } else {
            ImGui::Text(formatTimestampTimeSplit(splitTime[loopCounter]).c_str());
        }
        ImGui::TableNextColumn();
        // +/-
        if (splitStatus[loopCounter] == 0) {
            if (splitPreviousBest[loopCounter] == 0) {
                ImGui::TextColored(COLOR_WHITE, formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME).c_str());
            } else {
                if (GAMEPLAYSTAT_TOTAL_TIME < splitPreviousBest[loopCounter]) {
                ImGui::TextColored(COLOR_LIGHT_GREEN, formatTimestampTimeSplit(splitPreviousBest[loopCounter] - GAMEPLAYSTAT_TOTAL_TIME).c_str());
                } else if (GAMEPLAYSTAT_TOTAL_TIME == splitPreviousBest[loopCounter]) {
                    ImGui::TextColored(COLOR_WHITE, formatTimestampTimeSplit(splitPreviousBest[loopCounter]).c_str());
                } else if (GAMEPLAYSTAT_TOTAL_TIME > splitPreviousBest[loopCounter]) {
                    ImGui::TextColored(COLOR_LIGHT_RED, formatTimestampTimeSplit(GAMEPLAYSTAT_TOTAL_TIME - splitPreviousBest[loopCounter]).c_str());
                }
            }
        } else {
            if (splitPreviousBest[loopCounter] == 0) {
                ImGui::TextColored(COLOR_GREEN, formatTimestampTimeSplit(splitTime[loopCounter]).c_str());
            } else {
                if (splitTime[loopCounter] < splitPreviousBest[loopCounter]) {
                    ImGui::TextColored(COLOR_GREEN, formatTimestampTimeSplit(splitPreviousBest[loopCounter] - splitTime[loopCounter]).c_str());
                } else if (splitTime[loopCounter] == splitPreviousBest[loopCounter]) {
                    ImGui::TextColored(COLOR_WHITE, formatTimestampTimeSplit(splitTime[loopCounter] - splitPreviousBest[loopCounter]).c_str());
                } else if (splitTime[loopCounter] > splitPreviousBest[loopCounter]) {
                    ImGui::TextColored(COLOR_RED, formatTimestampTimeSplit(splitTime[loopCounter] - splitPreviousBest[loopCounter]).c_str());
                }
            }
        }
        ImGui::TableNextColumn();
        // Previous Best
        ImGui::Text(formatTimestampTimeSplit(splitPreviousBest[loopCounter]).c_str());
    
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
        splitTime.push_back(0);
        splitPreviousBest.push_back(0);
        splitBest.push_back(100000);
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
        splitPreviousBest.clear();
        status = "List has been reset";
        statusColor = COLOR_RED;
    }
    ImGui::SameLine(0);
    if (ImGui::Button("Save List")) {
        json j;
        j["splitItem"] = splitItem;
        j["splitTime"] = splitTime;
        j["splitPreviousBest"] = splitPreviousBest;
        j["backgroundColor.r"] = colorChoice.x;
        j["backgroundColor.g"] = colorChoice.y;
        j["backgroundColor.b"] = colorChoice.z;
        j["backgroundColor.a"] = colorChoice.w;
        std::ofstream file("SplitofHarkinian.json");
        file << j.dump(4);
        file.close();
        status = "List has been saved to disk";
        statusColor = COLOR_LIGHT_BLUE;
    }
    ImGui::SameLine(0);
    if (ImGui::Button("Load List")) {
        std::ifstream file("SplitofHarkinian.json");
        json j;

        if (file.is_open()) {
            file >> j;
            file.close();
        }
        int itemSize = j["splitItem"].size();
        splitItem.clear();
        splitTime.clear();
        splitPreviousBest.clear();
        splitBest.clear();
        splitStatus.clear();

        for (int i = 0; i < itemSize; i++) {
            splitItem.push_back(0);
            splitTime.push_back(0);
            splitPreviousBest.push_back(0);
            splitBest.push_back(100000);
            splitStatus.push_back(0);
        }
        splitItem = j["splitItem"].get<std::vector<uint32_t>>();
        splitTime = j["splitTime"].get<std::vector<uint32_t>>();
        splitPreviousBest = j["splitPreviousBest"].get<std::vector<uint32_t>>();
        colorChoice.x = j["backgroundColor.r"];
        colorChoice.y = j["backgroundColor.g"];
        colorChoice.z = j["backgroundColor.b"];
        colorChoice.w = j["backgroundColor.a"];
        file.close();
        status = "List has been loaded from Save Data";
        statusColor = COLOR_LIGHT_BLUE;
    }
    if (ImGui::Button("New Attempt")) {
        splitStatus.clear();
        splitTime.clear();
        for (int i = 0; i < splitItem.size(); i++) {
            splitStatus.push_back(0);
            splitTime.push_back(0);
        }
        splitCurNum++;
        std::stringstream ss;
        ss << splitCurNum;
        splitAttempt = (splitNumDisp).c_str() + ss.str();
        DrawTimeSplitSplits();
    }
    ImGui::SameLine();
    if (ImGui::Button("Update Splits")) {
        for (int i = 0; i < splitItem.size(); i++) {
            if (splitPreviousBest[i] == 0) {
                splitPreviousBest[i] = splitBest[i];
            } else if (splitPreviousBest[i] > splitBest[i]) {
                splitPreviousBest[i] = splitBest[i];
            }
        }
        DrawTimeSplitSplits();
    }

    if (ImGui::ColorEdit4("Background Color", (float*)&colorChoice, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
            Color_RGBA8 color;
            color.r = colorChoice.x;
            color.g = colorChoice.y;
            color.b = colorChoice.z;
            color.a = colorChoice.w;
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
            colorChoice = { 0.0f, 0.0f, 0.0f, 1.0f };
    }
    ImGui::TextColored(statusColor, status.c_str());
    UIWidgets::PaddedSeparator();
}

void InitializeSplitFile() {
    if (!std::filesystem::exists("SplitofHarkinian.json")) {
        json j;
        j["splitItem"] = splitItem;
        j["splitTime"] = splitTime;
        j["splitPreviousBest"] = splitPreviousBest;
        j["backgroundColor.r"] = colorChoice.x;
        j["backgroundColor.g"] = colorChoice.y;
        j["backgroundColor.b"] = colorChoice.z;
        j["backgroundColor.a"] = colorChoice.w;
        std::ofstream file("SplitofHarkinian.json");
        file << j.dump(4);
        file.close();
    }
}

static bool initialized = false;

void TimeSplitWindow::DrawElement() {
    if (!initialized) {
        InitializeSplitFile();
        initialized = true;
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, colorChoice);
    if (!ImGui::Begin("Time Splitter Window", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        ImGui::PopStyleColor();
        return;
    }
    if (ImGui::CollapsingHeader("Time Splitter")) {
        DrawTimeSplitOptions();
    }
    DrawTimeSplitSplits();
    ImGui::End();
    ImGui::PopStyleColor();
}

void TimeSplitWindow::InitElement() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
        TimeSplitSplitsHandler(itemEntry);
    });
}