//
//  GameMenuBar.cpp
//  soh
//
//  Created by David Chavez on 24.08.22.
//

#include "GameMenuBar.hpp"

#include <spdlog/spdlog.h>
#include <ImGui/imgui.h>
#include <libultraship/ImGuiImpl.h>
#include <libultraship/Cvar.h>

// TODO: Look into places that used to call `needs_save`

namespace UIWidgets {
    void Spacer(float height) {
        ImGui::Dummy(ImVec2(0.0f, height));
    }
}

namespace GameMenuBar {

    // MARK: - Helpers

    std::string GetWindowButtonText(const char* text, bool menuOpen) {
        char buttonText[100] = "";
        if(menuOpen) { strcat(buttonText,"> "); }
        strcat(buttonText, text);
        if (!menuOpen) { strcat(buttonText, "  "); }
        return buttonText;
    }

    // MARK: - Delegates

    void Draw() {
        if (ImGui::BeginMenu("Randomizer"))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12.0f, 6.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.22f, 0.38f, 0.56f, 1.0f));
            static ImVec2 buttonSize(200.0f, 0.0f);
            if (ImGui::Button(GetWindowButtonText("Randomizer Settings", CVar_GetS32("gRandomizerSettingsEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gRandomizerSettingsEnabled", 0);
                CVar_SetS32("gRandomizerSettingsEnabled", !currentValue);
//                needs_save = true;
                SohImGui::EnableWindow("Randomizer Settings", CVar_GetS32("gRandomizerSettingsEnabled", 0));
            }
            UIWidgets::Spacer(0);
            if (ImGui::Button(GetWindowButtonText("Item Tracker", CVar_GetS32("gItemTrackerEnabled", 0)).c_str(), buttonSize))
            {
                bool currentValue = CVar_GetS32("gItemTrackerEnabled", 0);
                CVar_SetS32("gItemTrackerEnabled", !currentValue);
//                needs_save = true;
                SohImGui::EnableWindow("Item Tracker", CVar_GetS32("gItemTrackerEnabled", 0));
            }
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);

            ImGui::EndMenu();
        }
    }
}
