
#include "DisplayOverlay.h"
#include <spdlog/fmt/fmt.h>
#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/Context.h>
#include <ship/window/Window.h>

extern "C" {
#include "variables.h"
uint64_t GetUnixTimestamp();
}

#include "ShipUtils.h"
#include "interface/parameter_static/parameter_static.h"
#include "2s2h/Enhancements/Enhancements.h"

#include <fast/Fast3dGui.h>

float windowScale = 1.0f;
ImVec4 windowBG = ImVec4(0, 0, 0, 0.5f);
static constexpr ImVec4 tintColor = {};

void DrawInGameTimer(uint32_t timer, ImVec4 color = ImVec4(1, 1, 1, 1)) {
    float windowScale = MAX(CVarGetFloat("gDisplayOverlay.Scale", 1.0f), 1.0f);

    std::string timerStr = Ship_FormatTimeDisplay(timer);
    uint16_t textureIndex = 0;
    for (const auto c : timerStr) {
        if (c == ':' || c == '.') {
            textureIndex = 10;
        } else {
            textureIndex = c - '0';
        }
        if (c == '.') {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (8.0f * windowScale));
            ImGui::Image(
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName(digitList[textureIndex]),
                ImVec2(8.0f * windowScale, 8.0f * windowScale), ImVec2(0, 0.5f), ImVec2(1, 1), color, tintColor);
        } else {
            ImGui::Image(
                std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                    ->GetTextureByName(digitList[textureIndex]),
                ImVec2(8.0f * windowScale, 16.0f * windowScale), ImVec2(0, 0), ImVec2(1, 1), color, tintColor);
        }
        ImGui::SameLine(0, 0);
    }
}

void DisplayOverlayWindow::Draw() {
    if (!IsVisible() || !gPlayState) {
        return;
    }
    int displayOverlay = CVarGetInteger(CVAR_DISPLAY_OVERLAY_MODE, 0);
    if (displayOverlay == TIMER_DISPLAY_NONE) {
        return;
    }

    float windowScale = MAX(CVarGetFloat("gDisplayOverlay.Scale", 1.0f), 1.0f);
    ImVec4 windowBG = !CVarGetInteger("gDisplayOverlay.Background", 0) ? ImVec4(0, 0, 0, 0.5f) : ImVec4(0, 0, 0, 0);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, windowBG);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowFontScale(windowScale);

    ImGui::Image(std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                     ->GetTextureByName(gTimerClockIconTex),
                 ImVec2(16.0f * windowScale, 16.0f * windowScale));
    ImGui::SameLine(0, 10.0f);

    uint64_t timeToDisplay = 0;
    if (gSaveContext.save.shipSaveInfo.fileCompletedAt == 0) { // Player has not beaten the game
        switch (displayOverlay) {
            case TIMER_DISPLAY_RTA:
                timeToDisplay = (GetUnixTimestamp() - gSaveContext.save.shipSaveInfo.fileCreatedAt);
                break;
            case TIMER_DISPLAY_IGT:
                timeToDisplay = ((GetUnixTimestamp() - gSaveContext.shipSaveContext.lastTimeLog) +
                                 gSaveContext.save.shipSaveInfo.filePlaytime);
                break;
            default:
                break;
        }
        DrawInGameTimer(timeToDisplay / 100);
    } else { // Player has beaten the game
        switch (displayOverlay) {
            case TIMER_DISPLAY_RTA:
                timeToDisplay =
                    (gSaveContext.save.shipSaveInfo.fileCompletedAt - gSaveContext.save.shipSaveInfo.fileCreatedAt);
                break;
            case TIMER_DISPLAY_IGT:
                timeToDisplay = gSaveContext.save.shipSaveInfo.filePlaytime;
                break;
            default:
                break;
        }
        DrawInGameTimer(timeToDisplay / 100, ImVec4(0, 1, 0, 1));
    }

    ImGui::End();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(2);
}

void DisplayOverlayWindow::InitElement() {
}
