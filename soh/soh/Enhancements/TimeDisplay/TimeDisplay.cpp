#include "TimeDisplay.h"
#include "soh/Enhancements/gameplaystats.h"
#include <global.h>

#include "assets/textures/parameter_static/parameter_static.h"
#include "assets/soh_assets.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
uint64_t GetUnixTimestamp();
}

float fontScale = 1.0f;
ImVec4 windowBG = ImVec4(0, 0, 0, 0.5f);

std::vector<TimeObject> timeDisplayList = {
    { DISPLAY_IN_GAME_TIMER, CVAR_ENHANCEMENT("TimeDisplay.Timers.InGameTimer") },
    { DISPLAY_TIME_OF_DAY,   CVAR_ENHANCEMENT("TimeDisplay.Timers.TimeofDay") }
};

std::vector<TimeObject> activeTimers;

std::string convertDayTime(uint32_t dayTime) {
    uint32_t totalSeconds = 24 * 60 * 60;
    uint32_t ss = static_cast<uint32_t>(static_cast<double>(dayTime) * (totalSeconds - 1) / 65535);
    uint32_t hh = ss / 3600;
    uint32_t mm = (ss % 3600) / 60;
    return fmt::format("{:0>2}:{:0>2}", hh, mm);
}

std::string formatTimeDisplay(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    uint32_t ds = value % 10;
    return fmt::format("{}:{:0>2}:{:0>2}.{}", hh, mm, ss, ds);
}

std::string timeDisplayGetTime(uint32_t timeID) {
    std::string timeDisplayTime;
    switch (timeID) {
        case DISPLAY_IN_GAME_TIMER:
            timeDisplayTime = formatTimeDisplay(GAMEPLAYSTAT_TOTAL_TIME).c_str();
            break;
        case DISPLAY_TIME_OF_DAY:
            timeDisplayTime = convertDayTime(gSaveContext.dayTime).c_str();
            break;
        default:
            break;
    }
    return timeDisplayTime;
}

ImTextureID timeDisplayGetIcon(uint32_t timeID) {
    ImTextureID textureDisplay;
    switch (timeID) {
        case DISPLAY_IN_GAME_TIMER:
            textureDisplay = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("GAMEPLAY_TIMER");
            break;
        case DISPLAY_TIME_OF_DAY:
            if (gSaveContext.dayTime >= 17759 && !(gSaveContext.dayTime >= 49155)) {
                textureDisplay = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("DAY_TIME_TIMER");
            } else {
                textureDisplay = Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("NIGHT_TIME_TIMER");
            }
            break;
        default:
            break;
    }
    return textureDisplay;
}

void TimeDisplayUpdateDisplayOptions(uint32_t timeID, bool pushBack) {
    if (pushBack) {
        activeTimers.push_back(timeDisplayList[timeID]);
    } else {
        uint32_t index = 0;
        for (auto& check : activeTimers) {
            if (check.timeID == timeID) {
                activeTimers.erase(activeTimers.begin() + index);
                return;
            }
            index++;
        }
    }
}

void TimeDisplayWindow::Draw() {
    if (!CVarGetInteger(CVAR_WINDOW("TimeDisplayEnabled"), 0)) {
        return;
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBG);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    ImGui::Begin("TimerDisplay", nullptr, ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(fontScale);
	if (activeTimers.size() == 0) {
		ImGui::Text("No Enabled Timers...");
	} else {
		ImGui::BeginTable("Timer List", 2, ImGuiTableFlags_NoClip);
		for (auto& timers : activeTimers) {
			ImGui::PushID(timers.timeID);
            ImGui::TableNextColumn();
            ImGui::Image(timeDisplayGetIcon(timers.timeID), ImVec2(16.0f * fontScale, 16.0f * fontScale));
            ImGui::TableNextColumn();
			ImGui::Text(timeDisplayGetTime(timers.timeID).c_str());
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

void TimeDisplayInitTimers() {
    for (auto& update : timeDisplayList) {
        if (CVarGetInteger(update.timeEnable, 0)) {
            activeTimers.push_back(update);
        }
    }
}

void TimeDisplayWindow::InitElement() {
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("GAMEPLAY_TIMER", gClockIconTex, ImVec4(1, 1, 1, 1));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("DAY_TIME_TIMER", gSunIcoTex, ImVec4(1, 1, 1, 1));
    Ship::Context::GetInstance()->GetWindow()->GetGui()->LoadGuiTexture("NIGHT_TIME_TIMER", gMoonIcoTex, ImVec4(1, 1, 1, 1));

    TimeDisplayInitSettings();
    TimeDisplayInitTimers();
}
