#include "sohConsole.h"
#include "soh/OTRGlobals.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"

void SohConsoleWindow::InitElement() {
    ConsoleWindow::InitElement();
}

void SohConsoleWindow::UpdateElement() {
    ConsoleWindow::UpdateElement();
}

void SohConsoleWindow::DrawElement() {
    UIWidgets::PushStyleInput(THEME_COLOR);
    // Small font (13) to match hardcoded width values in the LUS window.. set large font after below TODO addressed
    ImGui::PushFont(OTRGlobals::Instance->fontMonoSmall);

    // TODO: This can be removed after the LUS console window is designed better without hardcoding widths
    ImGui::BeginChild("##Console Wrapper", ImVec2(0, 0), ImGuiChildFlags_None,
                      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                          ImGuiWindowFlags_NoScrollWithMouse);

    ConsoleWindow::DrawElement();

    ImGui::EndChild();

    ImGui::PopFont();
    UIWidgets::PopStyleInput();
}
