#include "ImGuiHelpers.h"
#include "../../../../libultraship/libultraship/ImGuiImpl.h"

// Adds a text tooltip for the previous ImGui item
void SetLastItemHoverText(const std::string& text) {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(SohImGui::BreakTooltip(text, 60).c_str());
        ImGui::EndTooltip();
    }
}

// Adds a "?" next to the previous ImGui item with a custom tooltip
void InsertHelpHoverText(const std::string& text) {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "?");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(SohImGui::BreakTooltip(text, 60).c_str());
        ImGui::EndTooltip();
    }
}
