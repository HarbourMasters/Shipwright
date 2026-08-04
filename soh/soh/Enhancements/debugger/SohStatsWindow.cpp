#include "SohStatsWindow.h"
#include "soh/OTRGlobals.h"

void SohStatsWindow::DrawElement() {
    const float framerate = ImGui::GetIO().Framerate;
    const float deltatime = ImGui::GetIO().DeltaTime;
    ImGui::PushFont(OTRGlobals::Instance->fontMonoLarger);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

#if defined(_WIN32)
    ImGui::Text("Platform: Windows");
#elif defined(__IOS__)
    ImGui::Text("Platform: iOS");
#elif defined(__APPLE__)
    ImGui::Text("Platform: macOS");
#elif defined(__linux__)
    ImGui::Text("Platform: Linux");
#else
    ImGui::Text("Platform: Unknown");
#endif
    ImGui::Text("Status: %.3f ms/frame (%.1f FPS)", deltatime * 1000.0f, framerate);
    ImGui::PopStyleColor();
    ImGui::PopFont();
}
