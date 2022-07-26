#include "ImGuiHelpers.h"

// Automatically add newlines to break up tooltips longer than a specified number of characters
// Manually included newlines will still be respected and reset the line length
std::string BreakTooltip(const char* text) {
    const int lineLength = 60;
    std::string newText = std::string(text);
    const int tipLength = newText.length();
    int lastSpace = -1;
    int currentLineLength = 0;
    for (int currentCharacter = 0; currentCharacter < tipLength; currentCharacter++) {
        if (newText[currentCharacter] == '\n') {
            currentLineLength = 0;
            lastSpace = -1;
            continue;
        } 
        else if (newText[currentCharacter] == ' ') {
            lastSpace = currentCharacter;
        }
        
        if ((currentLineLength >= lineLength) && (lastSpace >= 0)) {
            newText[lastSpace] = '\n';
            currentLineLength = currentCharacter - lastSpace - 1;
            lastSpace = -1;
        }
        currentLineLength++;
    }
    return newText;
}

// Adds a text tooltip for the previous ImGui item
void SetLastItemHoverText(const std::string& text) {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(BreakTooltip(text.c_str()).c_str());
        ImGui::EndTooltip();
    }
}

// Adds a "?" next to the previous ImGui item with a custom tooltip
void InsertHelpHoverText(const std::string& text) {
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "?");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text(BreakTooltip(text.c_str()).c_str());
        ImGui::EndTooltip();
    }
}
