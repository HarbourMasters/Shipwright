#include "ImGuiHelpers.h"
#include "Lib/ImGui/imgui_internal.h"

// Adds a text tooltip for the previous ImGui item
void SetLastItemHoverText(const std::string& text) {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(text.c_str());
        ImGui::EndTooltip();
    }
}

// Adds a "?" next to the previous ImGui item with a custom tooltip
void InsertHelpHoverText(const std::string& text) {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "?");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(text.c_str());
        ImGui::EndTooltip();
    }
}

void PaddedSeparator(bool topPadding, bool bottomPadding, float x, float y) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (topPadding) {
        ImGui::Dummy(ImVec2(x, y));
    }
    ImGui::Separator();
    if (bottomPadding) {
        ImGui::Dummy(ImVec2(x, y));
    }
}