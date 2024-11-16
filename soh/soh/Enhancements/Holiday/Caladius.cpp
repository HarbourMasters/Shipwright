#include "Caladius.h"
#include "Holiday.hpp"
#include "soh/Notification/Notification.h"
#include "soh/Enhancements/gameplaystats.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
uint64_t GetUnixTimestamp();
}

#define AUTHOR "Caladius"
#define CVAR(v) "gHoliday." AUTHOR "." v

bool isDisabled = false;
float fontScale = 1.0f;

std::string formatTimestampIceTrapFever(uint32_t value) {
    uint32_t sec = value / 10;
    uint32_t hh = sec / 3600;
    uint32_t mm = (sec - hh * 3600) / 60;
    uint32_t ss = sec - hh * 3600 - mm * 60;
    return fmt::format("{}:{:0>2}:{:0>2}", hh, mm, ss);
}

void OnTimeOver() {
    gSaveContext.health = 0;
}

int32_t calculateRemainingTime() {
    int32_t timeRemaining = 
        ((gSaveContext.sohStats.count[COUNT_ICE_TRAPS] * (CVarGetInteger(CVAR("ExtendTimer"), 0) * 600)) +
            (CVarGetInteger(CVAR("StartTimer"), 0) * 600) - GAMEPLAYSTAT_TOTAL_TIME);
    if (timeRemaining <= 0) {
        OnTimeOver();
        timeRemaining = 0;
    }
    return timeRemaining;
}

static void OnConfigurationChanged() {
    isDisabled = !CVarGetInteger(CVAR("Enabled"), 0);
    fontScale = CVarGetFloat(CVAR("FontScale"), 1.0f);
    if (fontScale < 1.0f) {
        fontScale = 1.0f;
    }
    if (CVarGetInteger(CVAR("ExtendTimer"), 0) < 1) {
        CVarSetInteger(CVAR("ExtendTimer"), 1);
    }
    if (CVarGetInteger(CVAR("StartTimer"), 0) < 1) {
        CVarSetInteger(CVAR("StartTimer"), 1);
    }
}

void CaladiusWindow::Draw() {
    if (!CVarGetInteger(CVAR("Enabled"), 0)) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
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
    ImGui::Text(formatTimestampIceTrapFever(calculateRemainingTime()).c_str());
    ImGui::End();

    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(1);
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);
    if (UIWidgets::EnhancementCheckbox("Holiday Fever", CVAR("Enabled"))) {
        OnConfigurationChanged();
    }
    UIWidgets::Tooltip("Can you beat your objective before the Fever sets in?\n"
                       "- Obtaining Ice Traps extends your timer.");
    ImGui::Text("Options");
    if (UIWidgets::PaddedEnhancementSliderFloat("", "##FontScale", CVAR("FontScale"), 
        1.0f, 5.0f, "%.1fx", 1.0f, false, false, false, false, isDisabled)) {
        OnConfigurationChanged();
    }
    UIWidgets::PaddedEnhancementSliderInt("Starting Timer: %d minutes", "##StartTime", CVAR("StartTimer"),
        5, 30, "", 15, true, true, false, isDisabled);
    UIWidgets::PaddedEnhancementSliderInt("Time Extensions: %d minutes", "##ExtendTime", CVAR("ExtendTimer"),
        1, 10, "", 5, true, true, false, isDisabled);
}

static void RegisterMod() {
    OnConfigurationChanged();
}

static Holiday holiday(DrawMenu, RegisterMod);
