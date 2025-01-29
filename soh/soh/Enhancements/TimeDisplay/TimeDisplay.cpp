#include "TimeDisplay.h"
#include "soh/Enhancements/gameplaystats.h"
#include <global.h>

#include "assets/textures/parameter_static/parameter_static.h"
#include "assets/soh_assets.h"
#include "soh/SohGui/ImGuiUtils.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
uint64_t GetUnixTimestamp();
}

float fontScale = 1.0f;
std::string timeDisplayTime = "";
ImTextureID textureDisplay = 0;
ImVec4 windowBG = ImVec4(0, 0, 0, 0.5f);
ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

// ImVec4 Colors
#define COLOR_WHITE ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_LIGHT_RED ImVec4(1.0f, 0.05f, 0, 1.0f)
#define COLOR_LIGHT_BLUE ImVec4(0, 0.88f, 1.0f, 1.0f)
#define COLOR_LIGHT_GREEN ImVec4(0.52f, 1.0f, 0.23f, 1.0f)
#define COLOR_GREY ImVec4(0.78f, 0.78f, 0.78f, 1.0f)

const static std::vector<std::pair<std::string, const char*>> digitList = {
    { "DIGIT_0_TEXTURE", gCounterDigit0Tex }, { "DIGIT_1_TEXTURE", gCounterDigit1Tex },
    { "DIGIT_2_TEXTURE", gCounterDigit2Tex }, { "DIGIT_3_TEXTURE", gCounterDigit3Tex },
    { "DIGIT_4_TEXTURE", gCounterDigit4Tex }, { "DIGIT_5_TEXTURE", gCounterDigit5Tex },
    { "DIGIT_6_TEXTURE", gCounterDigit6Tex }, { "DIGIT_7_TEXTURE", gCounterDigit7Tex },
    { "DIGIT_8_TEXTURE", gCounterDigit8Tex }, { "DIGIT_9_TEXTURE", gCounterDigit9Tex },
    { "COLON_TEXTURE", gCounterColonTex },
};

const std::vector<TimeObject> timeDisplayList = {
    { DISPLAY_IN_GAME_TIMER, "Display Gameplay Timer", CVAR_ENHANCEMENT("TimeDisplay.Timers.InGameTimer") },
    { DISPLAY_TIME_OF_DAY, "Display Time of Day", CVAR_ENHANCEMENT("TimeDisplay.Timers.TimeofDay") },
    { DISPLAY_CONDITIONAL_TIMER, "Display Conditional Timer", CVAR_ENHANCEMENT("TimeDisplay.Timers.HotWater") },
    { DISPLAY_NAVI_TIMER, "Display Navi Timer", CVAR_ENHANCEMENT("TimeDisplay.Timers.NaviTimer") }
};

static std::vector<TimeObject> activeTimers;

std::string convertDayTime(uint32_t dayTime) {
    uint32_t totalSeconds = 24 * 60 * 60;
    uint32_t ss = static_cast<uint32_t>(static_cast<double>(dayTime) * (totalSeconds - 1) / 65535);
    uint32_t hh = ss / 3600;
    uint32_t mm = (ss % 3600) / 60;
    return fmt::format("{:0>2}:{:0>2}", hh, mm);
}

std::string convertNaviTime(uint32_t value) {
    uint32_t totalSeconds = value * 0.05;
    uint32_t ss = totalSeconds % 60;
    uint32_t mm = totalSeconds / 60;
    return fmt::format("{:0>2}:{:0>2}", mm, ss);
}

std::string formatHotWaterDisplay(uint32_t value) {
    uint32_t ss = value % 60;
    uint32_t mm = value / 60;
    return fmt::format("{:0>2}:{:0>2}", mm, ss);
}

std::string formatTimeDisplay(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

static void TimeDisplayGetTimer(uint32_t timeID) {
    timeDisplayTime = "";
    textureDisplay = 0;
    textColor = COLOR_WHITE;

    Player* player = GET_PLAYER(gPlayState);
    uint32_t timer1 = gSaveContext.timer1Value;

    switch (timeID) {
        case DISPLAY_IN_GAME_TIMER:
            textureDisplay = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("GAMEPLAY_TIMER");
            timeDisplayTime = formatTimeDisplay(GAMEPLAYSTAT_TOTAL_TIME).c_str();
            break;
        case DISPLAY_TIME_OF_DAY:
            if (gSaveContext.dayTime >= DAY_BEGINS && gSaveContext.dayTime < NIGHT_BEGINS) {
                textureDisplay =
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("DAY_TIME_TIMER");
            } else {
                textureDisplay =
                    Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("NIGHT_TIME_TIMER");
            }
            timeDisplayTime = convertDayTime(gSaveContext.dayTime).c_str();
            break;
        case DISPLAY_CONDITIONAL_TIMER:
            if (gSaveContext.timer1State > 0) {
                timeDisplayTime = formatHotWaterDisplay(gSaveContext.timer1Value).c_str();
                textColor =
                    gSaveContext.timer1State <= 4
                        ? (gPlayState->roomCtx.curRoom.behaviorType2 == ROOM_BEHAVIOR_TYPE2_3 ? COLOR_LIGHT_RED
                                                                                              : COLOR_LIGHT_BLUE)
                        : COLOR_WHITE;
                if (gSaveContext.timer1State <= 4) {
                    textureDisplay = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                        gPlayState->roomCtx.curRoom.behaviorType2 == ROOM_BEHAVIOR_TYPE2_3
                            ? itemMapping[ITEM_TUNIC_GORON].name
                            : itemMapping[ITEM_TUNIC_ZORA].name);
                }
                if (gSaveContext.timer1State >= 6) {
                    textureDisplay = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                        itemMapping[ITEM_SWORD_MASTER].name);
                }
            } else {
                textureDisplay = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                    itemMapping[ITEM_TUNIC_KOKIRI].name);
                timeDisplayTime = "-:--";
            }
            break;
        case DISPLAY_NAVI_TIMER:
            if (gSaveContext.naviTimer <= NAVI_PREPARE) {
                timeDisplayTime = convertNaviTime(NAVI_PREPARE - gSaveContext.naviTimer).c_str();
            } else if (gSaveContext.naviTimer <= NAVI_ACTIVE) {
                timeDisplayTime = convertNaviTime(NAVI_ACTIVE - gSaveContext.naviTimer).c_str();
                textColor = COLOR_LIGHT_GREEN;
            } else {
                timeDisplayTime = convertNaviTime(NAVI_COOLDOWN - gSaveContext.naviTimer).c_str();
                textColor = COLOR_GREY;
            }
            textureDisplay = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("NAVI_TIMER");
            break;
        default:
            break;
    }
}

void TimeDisplayUpdateDisplayOptions() {
    activeTimers.clear();
    for (auto& timer : timeDisplayList) {
        if (CVarGetInteger(timer.timeEnable, 0)) {
            activeTimers.push_back(timer);
        }
    }

    //if (pushBack) {
    //    activeTimers.push_back(timeDisplayList[timeID]);
    //} else {
    //    uint32_t index = 0;
    //    for (auto& check : activeTimers) {
    //        if (check.timeID == timeID) {
    //            activeTimers.erase(activeTimers.begin() + index);
    //            return;
    //        }
    //        index++;
    //    }
    //}
}

void TimeDisplayWindow::Draw() {
    if (!gPlayState) {
        return;
    }
    if (!CVarGetInteger(CVAR_WINDOW("TimeDisplayEnabled"), 0)) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBG);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    ImGui::Begin("TimerDisplay", nullptr,
                    ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(fontScale);
    if (activeTimers.size() == 0) {
        ImGui::Text("No Enabled Timers...");
    } else {
        ImGui::BeginTable("Timer List", 2, ImGuiTableFlags_NoClip);
        for (auto& timers : activeTimers) {
            ImGui::PushID(timers.timeID);
            TimeDisplayGetTimer(timers.timeID);
            ImGui::TableNextColumn();
            ImGui::Image(textureDisplay, ImVec2(16.0f * fontScale, 16.0f * fontScale));
            ImGui::TableNextColumn();

            if (timeDisplayTime != "-:--") {
                char* textToDecode = new char[timeDisplayTime.size() + 1];
                textToDecode = std::strcpy(textToDecode, timeDisplayTime.c_str());
                size_t textLength = timeDisplayTime.length();
                uint16_t textureIndex = 0;

                for (size_t i = 0; i < textLength; i++) {
                    if (textToDecode[i] == ':' || textToDecode[i] == '.') {
                        textureIndex = 10;
                    } else {
                        textureIndex = textToDecode[i] - '0';
                    }
                    if (textToDecode[i] == '.') {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (8.0f * fontScale));
                        ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                                         digitList[textureIndex].first),
                                     ImVec2(8.0f * fontScale, 8.0f * fontScale), ImVec2(0, 0.5f), ImVec2(1, 1),
                                     textColor, ImVec4(0, 0, 0, 0));
                    } else {
                        ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName(
                                         digitList[textureIndex].first),
                                     ImVec2(8.0f * fontScale, 16.0f * fontScale), ImVec2(0, 0), ImVec2(1, 1), textColor,
                                     ImVec4(0, 0, 0, 0));
                    }
                    ImGui::SameLine(0, 0);
                }
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(1);
}

void TimeDisplayInitSettings() {
    fontScale = CVarGetFloat(CVAR_ENHANCEMENT("TimeDisplay.FontScale"), 1.0f);
    if (fontScale < 1.0f) {
        fontScale = 1.0f;
    }
    if (CVarGetInteger(CVAR_ENHANCEMENT("TimeDisplay.ShowWindowBG"), 0)) {
        windowBG = ImVec4(0, 0, 0, 0);
    } else {
        windowBG = ImVec4(0, 0, 0, 0.5f);
    }
}

static void TimeDisplayInitTimers() {
    for (auto& update : timeDisplayList) {
        if (CVarGetInteger(update.timeEnable, 0)) {
            activeTimers.push_back(update);
        }
    }
}

void TimeDisplayWindow::InitElement() {
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("GAMEPLAY_TIMER", gClockIconTex, ImVec4(1, 1, 1, 1));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("DAY_TIME_TIMER", gSunIconTex, ImVec4(1, 1, 1, 1));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("NIGHT_TIME_TIMER", gMoonIconTex, ImVec4(1, 1, 1, 1));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("NAVI_TIMER", gNaviIconTex, ImVec4(1, 1, 1, 1));

    for (auto& load : digitList) {
        Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture(load.first.c_str(), load.second, ImVec4(1, 1, 1, 1));
    }

    TimeDisplayInitSettings();
    TimeDisplayInitTimers();
}
