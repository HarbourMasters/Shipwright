#include "HeapViewerWindow.hpp"

#include <spdlog/fmt/fmt.h>
#include <imgui.h>

#include <vector>
#include <set>

#include "soh/OTRGlobals.h"
#include "soh/ActorDB.h"

#include "soh/Enhancements/Restorations/N64MemoryModel/N64MemoryModel.hpp"

extern "C" {
#include "z64.h"
#include "functions.h"
#include "soh/Enhancements/Restorations/N64MemoryModel/n64_shadow_arena.h"
}

// -----------------------------------------------------------------------------------------------------------------
// Block data
// -----------------------------------------------------------------------------------------------------------------

struct BlockInfo {
    u32 offset;
    u32 size;
    bool isFree;
    u8 type;     // N64MEM_BLOCK_* (shadow only)
    s16 actorId; // Original actor ID (shadow only), -1 if unknown
    const char* actorName;
    bool isPinned;
    bool isGhost;  // Pinned block that was freed -- shown as a grayed-out row
    u32 heapIndex; // Original index in heap order (for block map cross-reference)
};

static std::vector<BlockInfo> sBlocks;
static std::vector<std::size_t> sDisplayOrder; // Indices into sBlocks: pinned first, then unpinned
static u32 sPinnedCount = 0;
static std::set<std::pair<s16, u8>> sPinnedBlocks; // {actorId, blockType} pairs
static u32 sAllocTotal = 0;
static u32 sFreeTotal = 0;
static u32 sLargestFree = 0;
static u32 sNodeCount = 0;
static u32 sArenaSize = 0;
static u32 sPreviousAlloc = 0;
static u32 sCycleCount = 0;
static s32 sLastDelta = 0;
static bool sIsShadow = false;

static const char* GetActorName(s16 actorId) {
    if (actorId < 0) {
        return nullptr;
    }

    const auto& entry = ActorDB::Instance->RetrieveEntry(actorId);
    return entry.desc.empty() ? entry.entry.name : entry.desc.c_str();
}

static const char* BlockTypeName(u8 type) {
    switch (type) {
        case N64MEM_BLOCK_INSTANCE:
            return "inst";

        case N64MEM_BLOCK_OVERLAY:
            return "ovl";

        case N64MEM_BLOCK_SUBSIDIARY:
            return "sub";

        case N64MEM_BLOCK_EFFECT:
            return "efx";

        case N64MEM_BLOCK_ABSOLUTE:
            return "abs";

        default:
            return "???";
    }
}

static const char* sEffectNames[] = {
    "Dust", "KiraKira", "Bomb", "Bomb2", "Blast", "G_Spk", "D_Fire", "Bubble",
    "(unset)", "G_Ripple", "G_Splash", "G_Magma", "G_Fire", "Lightning", "Dt_Bubble",
    "Hahen", "Stick", "Sibuki", "Sibuki2", "G_Magma2", "Stone1", "HitMark",
    "Fhg_Flash", "K_Fire", "Solder_Srch_Ball", "Kakera", "Ice_Piece", "En_Ice",
    "Fire_Tail", "En_Fire", "Extra", "Fcircle", "Dead_Db", "Dead_Dd", "Dead_Ds",
    "Dead_Sound", "Ice_Smoke",
};

static const char* GetBlockDisplayName(const BlockInfo& block) {
    if (block.isFree && !block.isGhost) {
        return nullptr;
    }

    if (block.type == N64MEM_BLOCK_EFFECT && block.actorId >= 0 &&
        block.actorId < static_cast<s16>(std::size(sEffectNames))) {
        return sEffectNames[block.actorId];
    }

    return block.actorName;
}

static void CollectZeldaArenaBlocks() {
    sBlocks.clear();
    sAllocTotal = 0;
    sFreeTotal = 0;
    sLargestFree = 0;
    sNodeCount = 0;
    sArenaSize = 0;
    sIsShadow = false;

    ArenaNode* node = ZeldaArena_GetHead();
    if (!node) {
        return;
    }

    while (node) {
        BlockInfo block;
        block.offset = static_cast<u32>(reinterpret_cast<uintptr_t>(node) + sizeof(ArenaNode));
        block.size = node->size;
        block.isFree = node->isFree;
        block.type = N64MEM_BLOCK_FREE;
        block.actorId = -1;
        block.actorName = nullptr;

        sBlocks.push_back(block);
        sArenaSize += sizeof(ArenaNode) + node->size;
        sNodeCount++;

        if (node->isFree) {
            sFreeTotal += node->size;
            if (node->size > sLargestFree) {
                sLargestFree = node->size;
            }
        } else {
            sAllocTotal += node->size;
        }

        node = node->next;
    }

    if (sPreviousAlloc != 0 && sAllocTotal != sPreviousAlloc) {
        sLastDelta = static_cast<s32>(sAllocTotal) - static_cast<s32>(sPreviousAlloc);
        ++sCycleCount;
    }

    sPreviousAlloc = sAllocTotal;
}

static void CollectShadowBlocks() {
    sBlocks.clear();
    sAllocTotal = 0;
    sFreeTotal = 0;
    sLargestFree = 0;
    sNodeCount = 0;
    sIsShadow = true;

    ShadowArena* shadow = N64Mem_GetShadowArena();
    if (!shadow) {
        sArenaSize = 0;
        return;
    }

    sArenaSize = ShadowArena_GetBufferSize(shadow);
    u32 offset = ShadowArena_GetHead(shadow);

    while (offset != SHADOW_NULL) {
        s32 isFree = 0;
        u32 size = 0;
        u32 next = SHADOW_NULL;

        if (!ShadowArena_GetNodeInfo(shadow, offset, &isFree, &size, &next)) {
            break;
        }

        BlockInfo block;
        block.offset = offset + shadow->nodeSize;
        block.size = size;
        block.isFree = isFree != 0;
        block.type = N64MEM_BLOCK_FREE;
        block.actorId = -1;
        block.actorName = nullptr;

        if (!isFree) {
            u8 type = 0;
            s16 actorId = -1;
            if (N64Mem_GetBlockInfo(block.offset, &type, &actorId)) {
                block.type = type;
                block.actorId = actorId;
                block.actorName = GetActorName(actorId);
            }
        }

        sBlocks.push_back(block);
        sNodeCount++;

        if (isFree) {
            sFreeTotal += size;
            if (size > sLargestFree) {
                sLargestFree = size;
            }
        } else {
            sAllocTotal += size;
        }

        offset = next;
    }

    if (sPreviousAlloc != 0 && sAllocTotal != sPreviousAlloc) {
        sLastDelta = static_cast<s32>(sAllocTotal) - static_cast<s32>(sPreviousAlloc);
        ++sCycleCount;
    }

    sPreviousAlloc = sAllocTotal;
}

// Build display order: assign heap indices, mark pinned blocks, add ghosts for freed pins, sort pinned to top.
static void BuildDisplayOrder() {
    sDisplayOrder.clear();
    sPinnedCount = 0;

    // Track which pin keys have a live block.
    std::set<std::pair<s16, u8>> matchedPins;

    for (std::size_t i = 0; i < sBlocks.size(); ++i) {
        sBlocks[i].heapIndex = static_cast<u32>(i);
        sBlocks[i].isGhost = false;
        std::pair key = { sBlocks[i].actorId, sBlocks[i].type };
        sBlocks[i].isPinned = !sBlocks[i].isFree && sPinnedBlocks.contains(key);
        if (sBlocks[i].isPinned) {
            matchedPins.insert(key);
        }
    }

    // Add ghost entries for pinned blocks that no longer exist.
    for (const auto& pin : sPinnedBlocks) {
        if (!matchedPins.contains(pin)) {
            BlockInfo ghost;
            ghost.offset = 0;
            ghost.size = 0;
            ghost.isFree = true;
            ghost.type = pin.second;
            ghost.actorId = pin.first;
            ghost.actorName = GetActorName(pin.first);
            ghost.isPinned = true;
            ghost.isGhost = true;
            ghost.heapIndex = 0;
            sBlocks.push_back(ghost);
        }
    }

    // Pinned first (heap order preserved within group, ghosts at end of pinned section).
    for (std::size_t i = 0; i < sBlocks.size(); ++i) {
        if (sBlocks[i].isPinned && !sBlocks[i].isGhost) {
            sDisplayOrder.push_back(i);
            ++sPinnedCount;
        }
    }

    for (std::size_t i = 0; i < sBlocks.size(); ++i) {
        if (sBlocks[i].isGhost) {
            sDisplayOrder.push_back(i);
            ++sPinnedCount;
        }
    }

    // Then unpinned.
    for (std::size_t i = 0; i < sBlocks.size(); ++i) {
        if (!sBlocks[i].isPinned) {
            sDisplayOrder.push_back(i);
        }
    }
}

// -----------------------------------------------------------------------------------------------------------------
// Colors
// -----------------------------------------------------------------------------------------------------------------

static ImU32 BlockColor(const BlockInfo& block) {
    if (block.isFree) {
        return IM_COL32(60, 180, 80, 255);
    }

    if (!sIsShadow) {
        return IM_COL32(200, 60, 60, 255);
    }

    switch (block.type) {
        case N64MEM_BLOCK_INSTANCE:
            return IM_COL32(200, 60, 60, 255);

        case N64MEM_BLOCK_OVERLAY:
            return IM_COL32(220, 140, 40, 255);

        case N64MEM_BLOCK_EFFECT:
            return IM_COL32(60, 120, 200, 255);

        case N64MEM_BLOCK_SUBSIDIARY:
            return IM_COL32(200, 200, 60, 255);

        case N64MEM_BLOCK_ABSOLUTE:
            return IM_COL32(160, 60, 200, 255);

        default:
            return IM_COL32(120, 120, 120, 255);
    }
}

static ImU32 ColorNode() {
    return IM_COL32(80, 80, 80, 255);
}

static ImU32 ColorBorder() {
    return IM_COL32(40, 40, 40, 255);
}

// -----------------------------------------------------------------------------------------------------------------
// Drawing
// -----------------------------------------------------------------------------------------------------------------

void HeapViewerWindow::DrawElement() {
    // Single arena view: Shadow when active, ZeldaArena when not.
    const bool isShadowArena = N64Mem_IsActive();
    const u32 nodeHeaderSize = isShadowArena ? N64Mem_GetShadowArena()->nodeSize : sizeof(ArenaNode);

    if (isShadowArena) {
        CollectShadowBlocks();
    } else {
        CollectZeldaArenaBlocks();
    }

    BuildDisplayOrder();

    if (isShadowArena) {
        if (ImGui::Button("Reset Tracking")) {
            sPreviousAlloc = 0;
            sCycleCount = 0;
            sLastDelta = 0;
        }

        if (!sPinnedBlocks.empty()) {
            ImGui::SameLine();
            if (ImGui::Button("Clear Pins")) {
                sPinnedBlocks.clear();
            }
        }
    }

    if (sBlocks.empty()) {
        ImGui::Text("Arena is not initialized.");
        return;
    }

    // --- Stats ---
    const f32 fragPercent = sFreeTotal > 0
                                ? (1.0f - static_cast<f32>(sLargestFree) / static_cast<f32>(sFreeTotal)) * 100.0f
                                : 0.0f;

    ImGui::Text("Arena: 0x%X (%u KB)  |  Nodes: %u", sArenaSize, sArenaSize / 1024, sNodeCount);
    ImGui::Text("Alloc: 0x%X (%u KB)  |  Free: 0x%X (%u KB)  |  Largest: 0x%X (%u KB)",
                sAllocTotal, sAllocTotal / 1024,
                sFreeTotal, sFreeTotal / 1024,
                sLargestFree, sLargestFree / 1024);

    ImGui::Text("Fragmentation: %.1f%%", fragPercent);
    ImGui::Text("Cycle: %u  |  Last delta: %s0x%X (%d bytes)",
                sCycleCount,
                sLastDelta >= 0 ? "+" : "-",
                static_cast<u32>(std::abs(sLastDelta)),
                sLastDelta);

    // --- Utilization bar ---
    const f32 utilization = sArenaSize > 0 ? static_cast<f32>(sAllocTotal) / static_cast<f32>(sArenaSize) : 0.0f;
    ImGui::ProgressBar(utilization, ImVec2(-1, 0),
                       fmt::format("{:.1f}% utilized", utilization * 100.0f).c_str());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Block map ---
    ImGui::Text("Block Map");

    constexpr f32 mapHeight = 40.0f;
    const f32 availWidth = ImGui::GetContentRegionAvail().x;
    const ImVec2 mapPos = ImGui::GetCursorScreenPos();

    ImGui::Dummy(ImVec2(availWidth, mapHeight));

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(mapPos, ImVec2(mapPos.x + availWidth, mapPos.y + mapHeight), ColorBorder());

    f32 xCursor = 0.0f;
    s32 hoveredBlock = -1;

    for (std::size_t i = 0; i < sBlocks.size(); ++i) {
        f32 nodeWidth = static_cast<f32>(nodeHeaderSize) / static_cast<f32>(sArenaSize) * availWidth;
        f32 blockWidth = static_cast<f32>(sBlocks[i].size) / static_cast<f32>(sArenaSize) * availWidth;

        if (nodeWidth < 1.0f) {
            nodeWidth = 1.0f;
        }

        if (blockWidth < 1.0f && sBlocks[i].size > 0) {
            blockWidth = 1.0f;
        }

        f32 x0 = mapPos.x + xCursor;
        f32 x1 = x0 + nodeWidth;
        drawList->AddRectFilled(ImVec2(x0, mapPos.y), ImVec2(x1, mapPos.y + mapHeight), ColorNode());
        xCursor += nodeWidth;

        x0 = mapPos.x + xCursor;
        x1 = x0 + blockWidth;
        drawList->AddRectFilled(ImVec2(x0, mapPos.y), ImVec2(x1, mapPos.y + mapHeight), BlockColor(sBlocks[i]));

        // White outline for pinned blocks.
        if (sBlocks[i].isPinned) {
            drawList->AddRect(ImVec2(x0, mapPos.y), ImVec2(x1, mapPos.y + mapHeight),
                              IM_COL32(255, 255, 255, 255), 0.0f, 0, 2.0f);
        }

        const f32 fullX0 = mapPos.x + xCursor - nodeWidth;
        ImVec2 blockMin(fullX0, mapPos.y);
        if (ImVec2 blockMax(x1, mapPos.y + mapHeight); ImGui::IsMouseHoveringRect(blockMin, blockMax)) {
            hoveredBlock = static_cast<s32>(i);
        }

        xCursor += blockWidth;
    }

    if (hoveredBlock >= 0) {
        const auto& block = sBlocks[hoveredBlock];
        ImGui::BeginTooltip();

        if (block.isFree) {
            ImGui::Text("Block %d: FREE", hoveredBlock);
        } else if (sIsShadow && GetBlockDisplayName(block)) {
            ImGui::Text("Block %d: %s (%s)", hoveredBlock, GetBlockDisplayName(block), BlockTypeName(block.type));
        } else if (sIsShadow) {
            ImGui::Text("Block %d: %s", hoveredBlock, BlockTypeName(block.type));
        } else {
            ImGui::Text("Block %d: ALLOCATED", hoveredBlock);
        }

        ImGui::Text("Offset: 0x%X", block.offset);
        ImGui::Text("Size: 0x%X (%u bytes)", block.size, block.size);
        ImGui::EndTooltip();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Block list ---
    ImGui::Text("Block List (%u blocks%s)", sNodeCount,
                sPinnedCount > 0 ? fmt::format(", {} pinned", sPinnedCount).c_str() : "");

    if (const s32 columnCount = sIsShadow ? 5 : 4; ImGui::BeginTable("##blocks", columnCount,
                                                                     ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                                                     ImGuiTableFlags_ScrollY |
                                                                     ImGuiTableFlags_Resizable,
                                                                     ImVec2(0, ImGui::GetContentRegionAvail().y))) {
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 40.0f);

        if (sIsShadow) {
            ImGui::TableSetupColumn("Actor", ImGuiTableColumnFlags_WidthStretch);
        }

        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupScrollFreeze(0, 1 + static_cast<s32>(sPinnedCount));
        ImGui::TableHeadersRow();

        for (std::size_t di = 0; di < sDisplayOrder.size(); ++di) {
            const std::size_t blockIdx = sDisplayOrder[di];
            const auto& block = sBlocks[blockIdx];
            ImGui::TableNextRow();

            // Tint pinned rows.
            if (block.isPinned) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1,
                                       block.isGhost ? IM_COL32(255, 60, 60, 15) : IM_COL32(255, 255, 255, 20));
            }

            // # -- Click to pin/unpin (allocated shadow blocks and ghosts)
            ImGui::TableNextColumn();

            if (sIsShadow && ((!block.isFree && block.actorId >= 0) || block.isGhost)) {
                ImGui::PushID(static_cast<int>(di));
                if (ImGui::Selectable(fmt::format("{}{}", block.isPinned ? "\xF0\x9F\x93\x8C " : "",
                                                  block.isGhost ? "--" : std::to_string(block.heapIndex)).c_str(),
                                      block.isPinned,
                                      ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap)) {
                    std::pair key = { block.actorId, block.type };

                    if (block.isPinned) {
                        sPinnedBlocks.erase(key);
                    } else {
                        sPinnedBlocks.insert(key);
                    }
                }
                ImGui::PopID();
            } else {
                ImGui::Text("%u", block.heapIndex);
            }

            // Actor (shadow only)
            if (sIsShadow) {
                ImGui::TableNextColumn();
                if (block.isGhost) {
                    const char* displayName = GetBlockDisplayName(block);
                    ImGui::TextDisabled("%s", displayName ? displayName : "???");
                } else if (block.isFree) {
                    ImGui::TextDisabled("--");
                } else if (const char* displayName = GetBlockDisplayName(block)) {
                    ImGui::Text("%s", displayName);
                    if (ImGui::IsItemHovered() && block.actorId >= 0 && block.type != N64MEM_BLOCK_EFFECT) {
                        const auto& entry = ActorDB::Instance->RetrieveEntry(block.actorId);
                        ImGui::SetTooltip("%s (0x%04X)", entry.entry.name, block.actorId);
                    }
                } else {
                    ImGui::TextDisabled("--");
                }
            }

            // Type
            ImGui::TableNextColumn();
            if (block.isGhost) {
                ImGui::TextColored(ImVec4(0.6f, 0.2f, 0.2f, 1.0f), "freed");
            } else if (block.isFree) {
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.3f, 1.0f), "free");
            } else if (sIsShadow) {
                const ImU32 color = BlockColor(block);
                ImVec4 colorVec = ImGui::ColorConvertU32ToFloat4(color);
                ImGui::TextColored(colorVec, "%s", BlockTypeName(block.type));
            } else {
                ImGui::TextColored(ImVec4(0.8f, 0.25f, 0.25f, 1.0f), "alloc");
            }

            // Size
            ImGui::TableNextColumn();
            if (block.isGhost) {
                ImGui::TextDisabled("--");
            } else {
                ImGui::Text("0x%X (%u)", block.size, block.size);
            }

            // Offset
            ImGui::TableNextColumn();
            if (block.isGhost) {
                ImGui::TextDisabled("--");
            } else {
                ImGui::Text("0x%X", block.offset);
            }
        }

        ImGui::EndTable();
    }
}