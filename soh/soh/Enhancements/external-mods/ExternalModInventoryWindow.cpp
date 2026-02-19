#include "ExternalModInventoryWindow.h"

#include <algorithm>

#include <imgui.h>

#include "ExternalModManager.h"

namespace SOH {

namespace {
constexpr int32_t kExternalModInventoryGridColumns = 8;
constexpr float kExternalModInventoryCellHeight = 42.0f;

const char* GetCButtonLabel(const int32_t cButtonIndex) {
    switch (cButtonIndex) {
        case 0:
            return "C-Left";
        case 1:
            return "C-Down";
        case 2:
            return "C-Right";
        default:
            return "C-?";
    }
}

std::string TruncateLabel(const std::string& value, const size_t maxLen) {
    if (value.size() <= maxLen) {
        return value;
    }

    if (maxLen <= 3) {
        return value.substr(0, maxLen);
    }

    return value.substr(0, maxLen - 3) + "...";
}

const char* GetSlotName(ExternalModItemSlot slot) {
    switch (slot) {
        case ExternalModItemSlot::Hookshot:
            return "SLOT_HOOKSHOT";
        case ExternalModItemSlot::Stick:
            return "SLOT_STICK";
        case ExternalModItemSlot::Bow:
            return "SLOT_BOW";
        case ExternalModItemSlot::FireArrow:
            return "SLOT_ARROW_FIRE";
        case ExternalModItemSlot::IceArrow:
            return "SLOT_ARROW_ICE";
        case ExternalModItemSlot::LightArrow:
            return "SLOT_ARROW_LIGHT";
        case ExternalModItemSlot::Hammer:
            return "SLOT_HAMMER";
        default:
            return "UNKNOWN";
    }
}
} // namespace

void ExternalModInventoryWindow::DrawElement() {
    auto& manager = ExternalModManager::Instance();
    const auto grid = manager.GetExtraInventoryGrid();

    const int32_t occupiedCount = static_cast<int32_t>(std::count_if(grid.begin(), grid.end(), [](const auto& cell) {
        return cell.occupied;
    }));

    ImGui::Text("Extra inventory grid: %d/%d", occupiedCount, static_cast<int32_t>(grid.size()));
    ImGui::TextDisabled("Hotkey: I  |  Drag/drop para reorganizar  |  Selecione item e equipe nos slots comuns");

    if (mSelectedCell >= static_cast<int32_t>(grid.size())) {
        mSelectedCell = -1;
    }

    if (grid.empty()) {
        ImGui::TextDisabled("Nenhum item de mod carregado.");
        return;
    }

    const auto tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings;
    if (ImGui::BeginTable("ExternalModInventoryGrid", kExternalModInventoryGridColumns, tableFlags)) {
        for (size_t i = 0; i < grid.size(); ++i) {
            if (i % kExternalModInventoryGridColumns == 0) {
                ImGui::TableNextRow();
            }

            ImGui::TableSetColumnIndex(static_cast<int32_t>(i % kExternalModInventoryGridColumns));
            const auto& cell = grid[i];
            const bool selected = mSelectedCell == static_cast<int32_t>(i);
            std::string label = cell.occupied ? cell.displayName : "(vazio)";
            if (label.empty()) {
                label = cell.occupied ? cell.itemId : "(vazio)";
            }
            label = TruncateLabel(label, 16);

            ImGui::PushID(static_cast<int32_t>(i));
            if (ImGui::Selectable(label.c_str(), selected, 0, ImVec2(-FLT_MIN, kExternalModInventoryCellHeight))) {
                mSelectedCell = static_cast<int32_t>(i);
                mStatusMessage.clear();
            }

            if (cell.occupied && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                const int32_t payloadIndex = static_cast<int32_t>(i);
                ImGui::SetDragDropPayload("ExternalModInventoryCell", &payloadIndex, sizeof(payloadIndex));
                ImGui::Text("Mover: %s", cell.displayName.c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const auto* payload = ImGui::AcceptDragDropPayload("ExternalModInventoryCell"); payload != nullptr) {
                    if (payload->DataSize == sizeof(int32_t)) {
                        const int32_t fromIndex = *reinterpret_cast<const int32_t*>(payload->Data);
                        const int32_t toIndex = static_cast<int32_t>(i);
                        std::string error;
                        if (!manager.MoveExtraInventoryCell(static_cast<size_t>(std::max(0, fromIndex)),
                                                            static_cast<size_t>(toIndex), error)) {
                            mStatusIsError = true;
                            mStatusMessage = error;
                        } else {
                            if (mSelectedCell == fromIndex) {
                                mSelectedCell = toIndex;
                            } else if (mSelectedCell == toIndex) {
                                mSelectedCell = fromIndex;
                            }
                            mStatusIsError = false;
                            mStatusMessage = "Posicao atualizada.";
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    if (!mStatusMessage.empty()) {
        const auto color = mStatusIsError ? ImVec4(0.95f, 0.35f, 0.35f, 1.0f) : ImVec4(0.45f, 0.85f, 0.45f, 1.0f);
        ImGui::TextColored(color, "%s", mStatusMessage.c_str());
    }

    ImGui::Separator();
    if (mSelectedCell < 0 || mSelectedCell >= static_cast<int32_t>(grid.size())) {
        ImGui::TextDisabled("Selecione um item no grid para equipar.");
        return;
    }

    const auto& selectedCell = grid[static_cast<size_t>(mSelectedCell)];
    if (!selectedCell.occupied) {
        ImGui::TextDisabled("Celula vazia selecionada.");
        return;
    }

    ImGui::Text("Item: %s", selectedCell.displayName.c_str());
    ImGui::TextDisabled("Mod: %s", selectedCell.modName.c_str());
    ImGui::TextDisabled("Slot: %s", GetSlotName(selectedCell.slot));

    for (int32_t cButton = 0; cButton < 3; ++cButton) {
        const std::string buttonLabel = std::string("Equipar ") + GetCButtonLabel(cButton);
        if (ImGui::Button(buttonLabel.c_str(), ImVec2(150.0f, 0.0f))) {
            std::string error;
            if (!manager.EquipExtraInventoryCellToButton(static_cast<size_t>(mSelectedCell), cButton, error)) {
                mStatusIsError = true;
                mStatusMessage = error;
            } else {
                mStatusIsError = false;
                mStatusMessage = std::string("Item equipado em ") + GetCButtonLabel(cButton) + ".";
            }
        }
        if (cButton < 2) {
            ImGui::SameLine();
        }
    }
}

} // namespace SOH
