#include "BenMenuBar.h"
#include <imgui.h>
#include "UIWidgets.hpp"
#include <string>

#include <fast/Fast3dGui.h>

namespace BenGui {

void DrawMenuBarIcon() {
    static bool gameIconLoaded = false;
    if (!gameIconLoaded) {
        std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->LoadTextureFromRawImage("Game_Icon", "textures/icons/g2ShipIcon.png");
        gameIconLoaded = true;
    }

    if (std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
            ->HasTextureByName("Game_Icon")) {
#ifdef __SWITCH__
        ImVec2 iconSize = ImVec2(20.0f, 20.0f);
        float posScale = 1.0f;
#elif defined(__WIIU__)
        ImVec2 iconSize = ImVec2(16.0f * 2, 16.0f * 2);
        float posScale = 2.0f;
#else
        ImVec2 iconSize = ImVec2(16.0f, 16.0f);
        float posScale = 1.0f;
#endif
        ImGui::SetCursorPos(ImVec2(5, 5) * posScale);
        ImGui::Image(std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                         ->GetTextureByName("Game_Icon"),
                     iconSize);
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(25, 0) * posScale);
    }
}

void DrawBenMenu() {
    if (UIWidgets::BeginMenu("2Ship")) {
        if (UIWidgets::MenuItem("Hide Menu Bar",
#if !defined(__SWITCH__) && !defined(__WIIU__)
                                "F1"
#else
                                "[-]"
#endif
                                )) {
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetMenuBar()->ToggleVisibility();
        }
#if !defined(__SWITCH__) && !defined(__WIIU__)
        if (UIWidgets::MenuItem("Toggle Fullscreen", "F11")) {
            Ship::Context::GetRawInstance()->GetWindow()->ToggleFullscreen();
        }
#endif
        if (UIWidgets::MenuItem("Reset",
#ifdef __APPLE__
                                "Command-R"
#elif !defined(__SWITCH__) && !defined(__WIIU__)
                                "Ctrl+R"
#else
                                ""
#endif
                                )) {
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                ->Dispatch("reset");
        }
#if !defined(__SWITCH__) && !defined(__WIIU__)
        if (UIWidgets::MenuItem("Open App Files Folder")) {
            std::string filesPath = Ship::Context::GetRawInstance()->GetAppDirectoryPath();
            SDL_OpenURL(std::string("file:///" + std::filesystem::absolute(filesPath).string()).c_str());
        }

        if (UIWidgets::MenuItem("Quit")) {
            Ship::Context::GetRawInstance()->GetWindow()->Close();
        }
#endif
        ImGui::EndMenu();
    }
}

void BenMenuBar::InitElement() {
}

void BenMenuBar::DrawElement() {
    if (ImGui::BeginMenuBar()) {
        DrawMenuBarIcon();

        static ImVec2 sWindowPadding(8.0f, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, sWindowPadding);

        ImGui::SetCursorPosY(0.0f);

        DrawBenMenu();

        ImGui::SetCursorPosY(0.0f);

        if (UIWidgets::BeginMenu("New Menu")) {
            ImGui::PushStyleColor(ImGuiCol_Text, UIWidgets::ColorValues.at(UIWidgets::Colors::Orange));
            ImGui::SeparatorText("NOTICE");
            ImGui::PopStyleColor();
            ImGui::Text(
                "All settings have now been moved to the new menu,\nwhich can be accessed with the Esc button.");
            ImGui::EndMenu();
        }

        ImGui::PopStyleVar(1);
        ImGui::EndMenuBar();
    }
}
} // namespace BenGui
