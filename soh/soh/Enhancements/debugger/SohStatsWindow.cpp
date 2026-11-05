#include "soh/Enhancements/Lang/Lang.h"
#include "SohStatsWindow.h"
#include "soh/OTRGlobals.h"

void SohStatsWindow::DrawElement() {
    const float framerate = ImGui::GetIO().Framerate;
    const float deltatime = ImGui::GetIO().DeltaTime;
    ImGui::PushFont(OTRGlobals::Instance->GetGuiFont(OTRGlobals::Instance->fontMonoLarger));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

#if defined(_WIN32)
    ImGui::TextUnformatted(
        Lang::UIString("ui.text.enhancements.debugger.sohstatswindow.platform_windows", "Platform: Windows"));
#elif defined(__IOS__)
    ImGui::TextUnformatted(
        Lang::UIString("ui.text.enhancements.debugger.sohstatswindow.platform_ios", "Platform: iOS"));
#elif defined(__APPLE__)
    ImGui::TextUnformatted(
        Lang::UIString("ui.text.enhancements.debugger.sohstatswindow.platform_macos", "Platform: macOS"));
#elif defined(__linux__)
    ImGui::TextUnformatted(
        Lang::UIString("ui.text.enhancements.debugger.sohstatswindow.platform_linux", "Platform: Linux"));
#else
    ImGui::TextUnformatted(
        Lang::UIString("ui.text.enhancements.debugger.sohstatswindow.platform_unknown", "Platform: Unknown"));
#endif
    ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.sohstatswindow.status_0_3f_ms_frame_0_1f_fps",
                               "Status: %0.3f ms/frame (%0.1f FPS)"),
                deltatime * 1000.0f, framerate);
    ImGui::PopStyleColor();
    ImGui::PopFont();
}
