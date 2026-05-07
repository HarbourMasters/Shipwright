#include "HeapViewerWindow.hpp"

#include <spdlog/fmt/fmt.h>
#include <imgui.h>

#include <vector>

#include "soh/OTRGlobals.h"

extern "C" {
#include "z64.h"
#include "functions.h"

#include "soh/Enhancements/Restorations/N64MemoryModel/N64MemoryModel.hpp"
#include "soh/Enhancements/Restorations/N64MemoryModel/ShadowArena/shadow_arena.h"

extern ArenaNode* ZeldaArena_GetHead();
}

struct BlockInfo
{
    u32 offset; // For shadow: offset into buffer. For ZeldaArena: not used.
    std::uintptr_t address;
    std::size_t size;
    bool isFree;
};

static std::vector<BlockInfo> sBlocks;
static u32 sAllocTotal = 0;
static u32 sFreeTotal = 0;
static u32 sLargestFree = 0;
static u32 sNodeCount = 0;
static u32 sArenaSize = 0;
static u32 sPreviousAlloc = 0;
static u32 sCycleCount = 0;
static s32 sLastDelta = 0;

// Shadow arena view state
static std::vector<BlockInfo> sShadowBlocks;
static u32 sShadowAllocTotal = 0;
static u32 sShadowFreeTotal = 0;
static u32 sShadowLargestFree = 0;
static u32 sShadowNodeCount = 0;
static u32 sShadowArenaSize = 0;
static u32 sShadowPreviousAlloc = 0;
static u32 sShadowCycleCount = 0;
static s32 sShadowLastDelta = 0;

static void CollectBlocks()
{
    sBlocks.clear();
    sAllocTotal = 0;
    sFreeTotal = 0;
    sLargestFree = 0;
    sNodeCount = 0;
    sArenaSize = 0;

    ArenaNode* node = ZeldaArena_GetHead();
    if (!node)
    {
        return;
    }

    while (node)
    {
        BlockInfo block;
        block.address = reinterpret_cast<uintptr_t>(node) + sizeof(ArenaNode);
        block.size = node->size;
        block.isFree = node->isFree;

        sBlocks.push_back(block);
        sArenaSize += sizeof(ArenaNode) + node->size;
        sNodeCount++;

        if (node->isFree)
        {
            sFreeTotal += node->size;
            if (node->size > sLargestFree)
            {
                sLargestFree = node->size;
            }
        }
        else
        {
            sAllocTotal += node->size;
        }

        node = node->next;
    }

    if (sPreviousAlloc != 0 && sAllocTotal != sPreviousAlloc)
    {
        sLastDelta = static_cast<s32>(sAllocTotal) - static_cast<s32>(sPreviousAlloc);
        ++sCycleCount;
    }
    sPreviousAlloc = sAllocTotal;
}

static void CollectShadowBlocks()
{
    sShadowBlocks.clear();
    sShadowAllocTotal = 0;
    sShadowFreeTotal = 0;
    sShadowLargestFree = 0;
    sShadowNodeCount = 0;

    ShadowArena* shadow = N64Mem_GetShadowArena();
    if (!shadow)
    {
        sShadowArenaSize = 0;
        return;
    }

    sShadowArenaSize = ShadowArena_GetBufferSize(shadow);
    u32 offset = ShadowArena_GetHead(shadow);

    while (offset != SHADOW_NULL)
    {
        s32 isFree = 0;
        u32 size = 0;
        u32 next = SHADOW_NULL;

        if (!ShadowArena_GetNodeInfo(shadow, offset, &isFree, &size, &next))
        {
            break;
        }

        BlockInfo block;
        block.offset = offset;
        block.address = offset + SHADOW_NODE_SIZE;
        block.size = size;
        block.isFree = isFree != 0;

        sShadowBlocks.push_back(block);
        sShadowNodeCount++;

        if (isFree)
        {
            sShadowFreeTotal += size;
            if (size > sShadowLargestFree)
            {
                sShadowLargestFree = size;
            }
        }
        else
        {
            sShadowAllocTotal += size;
        }

        offset = next;
    }

    if (sShadowPreviousAlloc != 0 && sShadowAllocTotal != sShadowPreviousAlloc)
    {
        sShadowLastDelta = static_cast<s32>(sShadowAllocTotal) - static_cast<s32>(sShadowPreviousAlloc);
        ++sShadowCycleCount;
    }
    sShadowPreviousAlloc = sShadowAllocTotal;
}

static ImU32 ColorAlloc()
{
    return IM_COL32(200, 60, 60, 255);
}

static ImU32 ColorFree()
{
    return IM_COL32(60, 180, 80, 255);
}

static ImU32 ColorNode()
{
    return IM_COL32(80, 80, 80, 255);
}

static ImU32 ColorBorder()
{
    return IM_COL32(40, 40, 40, 255);
}

static void DrawArenaView(const std::vector<BlockInfo>& blocks, u32 arenaSize, u32 allocTotal,
                          u32 freeTotal, u32 largestFree, u32 nodeCount, u32 cycleCount,
                          s32 lastDelta, u32 nodeSize, const char* id)
{
    if (blocks.empty())
    {
        ImGui::Text("Arena is not initialized.");
        return;
    }

    // --- Stats ---
    const f32 fragPercent = freeTotal > 0
                                ? (1.0f - static_cast<f32>(largestFree) / static_cast<f32>(freeTotal)) * 100.0f
                                : 0.0f;

    ImGui::Text("Arena: 0x%X (%u KB)  |  Nodes: %u", arenaSize, arenaSize / 1024, nodeCount);
    ImGui::Text("Alloc: 0x%X (%u KB)  |  Free: 0x%X (%u KB)  |  Largest: 0x%X (%u KB)",
                allocTotal, allocTotal / 1024,
                freeTotal, freeTotal / 1024,
                largestFree, largestFree / 1024);
    ImGui::Text("Fragmentation: %.1f%%", fragPercent);
    ImGui::Text("Cycle: %u  |  Last delta: %s0x%X (%d bytes)",
                cycleCount,
                lastDelta >= 0 ? "+" : "-",
                static_cast<u32>(std::abs(lastDelta)),
                lastDelta);

    // --- Utilization bar ---
    const f32 utilization = arenaSize > 0 ? static_cast<f32>(allocTotal) / static_cast<f32>(arenaSize) : 0.0f;
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

    for (std::size_t i = 0; i < blocks.size(); ++i)
    {
        f32 nodeWidth = static_cast<f32>(nodeSize) / static_cast<f32>(arenaSize) * availWidth;
        f32 blockWidth = static_cast<f32>(blocks[i].size) / static_cast<f32>(arenaSize) * availWidth;

        if (nodeWidth < 1.0f)
        {
            nodeWidth = 1.0f;
        }

        if (blockWidth < 1.0f && blocks[i].size > 0)
        {
            blockWidth = 1.0f;
        }

        f32 x0 = mapPos.x + xCursor;
        f32 x1 = x0 + nodeWidth;
        drawList->AddRectFilled(ImVec2(x0, mapPos.y), ImVec2(x1, mapPos.y + mapHeight), ColorNode());
        xCursor += nodeWidth;

        x0 = mapPos.x + xCursor;
        x1 = x0 + blockWidth;
        const ImU32 color = blocks[i].isFree ? ColorFree() : ColorAlloc();
        drawList->AddRectFilled(ImVec2(x0, mapPos.y), ImVec2(x1, mapPos.y + mapHeight), color);

        const f32 fullX0 = mapPos.x + xCursor - nodeWidth;
        ImVec2 blockMin(fullX0, mapPos.y);
        if (ImVec2 blockMax(x1, mapPos.y + mapHeight); ImGui::IsMouseHoveringRect(blockMin, blockMax))
        {
            hoveredBlock = static_cast<s32>(i);
        }

        xCursor += blockWidth;
    }

    if (hoveredBlock >= 0)
    {
        const auto& [offset, address, size, isFree] = blocks[hoveredBlock];
        ImGui::BeginTooltip();
        ImGui::Text("Block %d: %s", hoveredBlock, isFree ? "FREE" : "ALLOCATED");
        ImGui::Text("Offset: 0x%X", static_cast<u32>(address));
        ImGui::Text("Size: 0x%X (%u bytes)", static_cast<u32>(size), static_cast<u32>(size));
        ImGui::EndTooltip();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Block list ---
    ImGui::Text("Block List (%u blocks)", nodeCount);

    const std::string tableId = fmt::format("##blocks_{}", id);
    if (ImGui::BeginTable(tableId.c_str(), 4,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                          ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
                          ImVec2(0, ImGui::GetContentRegionAvail().y)))
    {
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Offset", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < blocks.size(); ++i)
        {
            const auto& [offset, address, size, isFree] = blocks[i];
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%zu", i);

            ImGui::TableNextColumn();
            if (isFree)
            {
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.3f, 1.0f), "FREE");
            }
            else
            {
                ImGui::TextColored(ImVec4(0.8f, 0.25f, 0.25f, 1.0f), "ALLOC");
            }

            ImGui::TableNextColumn();
            ImGui::Text("0x%X", static_cast<u32>(address));

            ImGui::TableNextColumn();
            ImGui::Text("0x%X (%u)", static_cast<u32>(size), static_cast<u32>(size));
        }

        ImGui::EndTable();
    }
}

void HeapViewerWindow::DrawElement()
{
    if (ImGui::BeginTabBar("##heap_tabs"))
    {
        if (ImGui::BeginTabItem("ZeldaArena"))
        {
            CollectBlocks();
            DrawArenaView(sBlocks, sArenaSize, sAllocTotal, sFreeTotal, sLargestFree,
                          sNodeCount, sCycleCount, sLastDelta,
                          sizeof(ArenaNode), "zelda");
            ImGui::EndTabItem();
        }

        if (N64Mem_IsActive())
        {
            if (ImGui::BeginTabItem("ShadowArena (N64)"))
            {
                CollectShadowBlocks();

                if (ImGui::Button("Reset Tracking"))
                {
                    sShadowPreviousAlloc = 0;
                    sShadowCycleCount = 0;
                    sShadowLastDelta = 0;
                }

                DrawArenaView(sShadowBlocks, sShadowArenaSize, sShadowAllocTotal,
                              sShadowFreeTotal, sShadowLargestFree, sShadowNodeCount,
                              sShadowCycleCount, sShadowLastDelta,
                              SHADOW_NODE_SIZE, "shadow");
                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }
}
