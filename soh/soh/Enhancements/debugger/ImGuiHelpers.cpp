#include "ImGuiHelpers.h"

// Automatically add newlines to break up tooltips longer than a specified number of characters
// Manually included newlines will still be respected and reset the line length
std::string BreakTooltip(const char* text) {
    const int lineLength = 60;
    std::string newText = std::string(text);
    const int tipLength = newText.length();
    int currentPointer = 0;
    while (currentPointer < tipLength) {
        if ((currentPointer + lineLength - 1) < tipLength) {
            int newline = -1;
            for (int i = 0; i < lineLength; i++) {
                if (newText[currentPointer + i] == '\n') {
                    newline = i;
                    break;
                }
            }
            if (newline >= 0) {
                currentPointer = currentPointer - lineLength + newline + 1;
            } else if (((currentPointer + lineLength) < tipLength) && (newText[currentPointer + lineLength] == ' ')) {
                newText[currentPointer + lineLength] = '\n';
                currentPointer++;
            } else {
                int space = -1;
                for (int j = lineLength - 1; j >= 0; j--) {
                    if (newText[currentPointer + j] == ' ') {
                        space = j;
                        break;
                    }
                }
                if (space >= 0) {
                    newText[currentPointer + space] = '\n';
                    currentPointer = currentPointer - lineLength + space + 1;
                }
            }
        }
        currentPointer += lineLength;
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
