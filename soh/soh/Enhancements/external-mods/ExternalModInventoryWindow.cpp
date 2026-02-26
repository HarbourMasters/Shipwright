#include "ExternalModInventoryWindow.h"

#include <algorithm>

#include <imgui.h>

#include "ExternalModManager.h"

namespace SOH {

namespace {
constexpr int32_t kExternalModInventoryGridColumns = 6;
constexpr int32_t kExternalModInventoryCellsPerPage = 24;
constexpr float kExternalModInventoryCellHeight = 42.0f;

const char* GetButtonLabel(const int32_t buttonIndex) {
    switch (buttonIndex) {
        case 1:
            return "C-Left";
        case 2:
            return "C-Down";
        case 3:
            return "C-Right";
        case 4:
            return "D-Up";
        case 5:
            return "D-Down";
        case 6:
            return "D-Left";
        case 7:
            return "D-Right";
        default:
            return "?";
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
        case ExternalModItemSlot::Stick:
            return "SLOT_STICK";
        case ExternalModItemSlot::Nut:
            return "SLOT_NUT";
        case ExternalModItemSlot::Bomb:
            return "SLOT_BOMB";
        case ExternalModItemSlot::Bow:
            return "SLOT_BOW";
        case ExternalModItemSlot::FireArrow:
            return "SLOT_ARROW_FIRE";
        case ExternalModItemSlot::DinsFire:
            return "SLOT_DINS_FIRE";
        case ExternalModItemSlot::Slingshot:
            return "SLOT_SLINGSHOT";
        case ExternalModItemSlot::Ocarina:
            return "SLOT_OCARINA";
        case ExternalModItemSlot::Bombchu:
            return "SLOT_BOMBCHU";
        case ExternalModItemSlot::Hookshot:
            return "SLOT_HOOKSHOT";
        case ExternalModItemSlot::IceArrow:
            return "SLOT_ARROW_ICE";
        case ExternalModItemSlot::FaroresWind:
            return "SLOT_FARORES_WIND";
        case ExternalModItemSlot::Boomerang:
            return "SLOT_BOOMERANG";
        case ExternalModItemSlot::Lens:
            return "SLOT_LENS";
        case ExternalModItemSlot::Bean:
            return "SLOT_BEAN";
        case ExternalModItemSlot::LightArrow:
            return "SLOT_ARROW_LIGHT";
        case ExternalModItemSlot::Hammer:
            return "SLOT_HAMMER";
        case ExternalModItemSlot::NayrusLove:
            return "SLOT_NAYRUS_LOVE";
        case ExternalModItemSlot::Bottle1:
            return "SLOT_BOTTLE_1";
        case ExternalModItemSlot::Bottle2:
            return "SLOT_BOTTLE_2";
        case ExternalModItemSlot::Bottle3:
            return "SLOT_BOTTLE_3";
        case ExternalModItemSlot::Bottle4:
            return "SLOT_BOTTLE_4";
        case ExternalModItemSlot::TradeAdult:
            return "SLOT_TRADE_ADULT";
        case ExternalModItemSlot::TradeChild:
            return "SLOT_TRADE_CHILD";
        default:
            return "UNKNOWN";
    }
}
} // namespace

void ExternalModInventoryWindow::DrawElement() {
    auto& manager = ExternalModManager::Instance();
    const auto allCells = manager.GetExtraInventoryGrid();
    const int32_t pageCount = std::max(1, manager.GetExtraInventoryPageCount());
    mCurrentPage = std::clamp(mCurrentPage, 0, pageCount - 1);

    const int32_t occupiedCount = static_cast<int32_t>(std::count_if(allCells.begin(), allCells.end(), [](const auto& cell) {
        return cell.occupied;
    }));

    ImGui::Text("Virtual inventory: %d/%d", occupiedCount, static_cast<int32_t>(allCells.size()));
    ImGui::TextDisabled("Hotkey: I  |  Drag/drop para reorganizar  |  Page + equip C/D");

    if (ImGui::Button("<")) {
        mCurrentPage = std::max(0, mCurrentPage - 1);
        mSelectedCell = -1;
    }
    ImGui::SameLine();
    ImGui::Text("Page %d / %d", mCurrentPage + 1, pageCount);
    ImGui::SameLine();
    if (ImGui::Button(">")) {
        mCurrentPage = std::min(pageCount - 1, mCurrentPage + 1);
        mSelectedCell = -1;
    }

    if (mSelectedCell >= kExternalModInventoryCellsPerPage) {
        mSelectedCell = -1;
    }

    if (allCells.empty()) {
        ImGui::TextDisabled("Nenhum item de mod carregado.");
        return;
    }

    const auto tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings;
    if (ImGui::BeginTable("ExternalModInventoryGrid", kExternalModInventoryGridColumns, tableFlags)) {
        for (int32_t i = 0; i < kExternalModInventoryCellsPerPage; ++i) {
            if (i % kExternalModInventoryGridColumns == 0) {
                ImGui::TableNextRow();
            }

            ImGui::TableSetColumnIndex(i % kExternalModInventoryGridColumns);
            ExternalModInventoryCellView cell;
            const bool hasCell = manager.GetExtraInventoryPageCell(mCurrentPage, i, cell);
            const bool selected = mSelectedCell == i;
            std::string label = cell.occupied ? cell.displayName : "(vazio)";
            if (label.empty()) {
                label = cell.occupied ? cell.itemId : "(vazio)";
            }
            label = TruncateLabel(label, 16);

            ImGui::PushID(i);
            if (ImGui::Selectable(label.c_str(), selected, 0, ImVec2(-FLT_MIN, kExternalModInventoryCellHeight))) {
                mSelectedCell = i;
                mStatusMessage.clear();
            }

            if (cell.occupied && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                const int32_t payloadIndex = static_cast<int32_t>(cell.index);
                ImGui::SetDragDropPayload("ExternalModInventoryCell", &payloadIndex, sizeof(payloadIndex));
                ImGui::Text("Mover: %s", cell.displayName.c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const auto* payload = ImGui::AcceptDragDropPayload("ExternalModInventoryCell"); payload != nullptr) {
                    if (payload->DataSize == sizeof(int32_t)) {
                        const int32_t fromIndex = *reinterpret_cast<const int32_t*>(payload->Data);
                        const int32_t toIndex =
                            hasCell ? static_cast<int32_t>(cell.index)
                                    : (mCurrentPage * kExternalModInventoryCellsPerPage) + static_cast<int32_t>(i);
                        std::string error;
                        if (!manager.MoveExtraInventoryCell(static_cast<size_t>(std::max(0, fromIndex)),
                                                            static_cast<size_t>(toIndex), error)) {
                            mStatusIsError = true;
                            mStatusMessage = error;
                        } else {
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
    if (mSelectedCell < 0 || mSelectedCell >= kExternalModInventoryCellsPerPage) {
        ImGui::TextDisabled("Selecione um item no grid para equipar.");
        return;
    }

    ExternalModInventoryCellView selectedCell;
    if (!manager.GetExtraInventoryPageCell(mCurrentPage, mSelectedCell, selectedCell) || !selectedCell.occupied) {
        ImGui::TextDisabled("Celula vazia selecionada.");
        return;
    }

    ImGui::Text("Item: %s", selectedCell.displayName.c_str());
    ImGui::TextDisabled("Mod: %s", selectedCell.modName.c_str());
    ImGui::TextDisabled("Placement: %s",
                        selectedCell.placement == ExternalModItemPlacement::Virtual ? "virtual" : GetSlotName(selectedCell.slot));

    for (int32_t buttonIndex = 1; buttonIndex <= 7; ++buttonIndex) {
        const uint8_t buttonMask = static_cast<uint8_t>(1u << (buttonIndex - 1));
        const bool canAssign = (selectedCell.assignableButtonsMask & buttonMask) != 0;
        if (!canAssign) {
            ImGui::BeginDisabled();
        }

        const std::string buttonLabel = std::string("Equipar ") + GetButtonLabel(buttonIndex);
        if (ImGui::Button(buttonLabel.c_str(), ImVec2(150.0f, 0.0f))) {
            std::string error;
            if (!manager.EquipExtraInventoryCellToButton(selectedCell.index, buttonIndex, error)) {
                mStatusIsError = true;
                mStatusMessage = error;
            } else {
                mStatusIsError = false;
                mStatusMessage = std::string("Item equipado em ") + GetButtonLabel(buttonIndex) + ".";
            }
        }
        if (!canAssign) {
            ImGui::EndDisabled();
        }

        if ((buttonIndex % 3) != 0 && buttonIndex < 7) {
            ImGui::SameLine();
        }
    }
}

} // namespace SOH
