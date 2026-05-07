#include "HeapViewerWindow.hpp"

#include <spdlog/fmt/fmt.h>

#include <vector>

#include "soh/OTRGlobals.h"

extern "C" {
#include "z64.h"
#include "functions.h"
}

struct BlockInfo
{
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

void HeapViewerWindow::DrawElement()
{
    CollectBlocks();

    if (sBlocks.empty())
    {
        ImGui::Text("ZeldaArena is not initialized.");
        return;
    }

    // --- Stats ----
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

    // Reserve space for the map
    ImGui::Dummy(ImVec2(availWidth, mapHeight));

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Background
    drawList->AddRectFilled(mapPos, ImVec2(mapPos.x + availWidth, mapPos.y + mapHeight), ColorBorder());

    // Draw blocks proportionally
    f32 xCursor = 0.0f;
    s32 hoveredBlock = -1;

    for (std::size_t i = 0; i < sBlocks.size(); ++i)
    {
        // Node header
        f32 nodeWidth = static_cast<f32>(sizeof(ArenaNode)) / static_cast<f32>(sArenaSize) * availWidth;
        // Block data
        f32 blockWidth = static_cast<f32>(sBlocks[i].size) / static_cast<f32>(sArenaSize) * availWidth;

        // Ensure minimum 1px visibility for non-zero blocks
        if (nodeWidth < 1.0f)
        {
            nodeWidth = 1.0f;
        }

        if (blockWidth < 1.0f && sBlocks[i].size > 0)
        {
            blockWidth = 1.0f;
        }

        // Node header segment (dark gray)
        f32 x0 = mapPos.x + xCursor;
        f32 x1 = x0 + nodeWidth;
        drawList->AddRectFilled(ImVec2(x0, mapPos.y), ImVec2(x1, mapPos.y + mapHeight), ColorNode());
        xCursor += nodeWidth;

        // Block data segment
        x0 = mapPos.x + xCursor;
        x1 = x0 + blockWidth;
        const ImU32 color = sBlocks[i].isFree ? ColorFree() : ColorAlloc();
        drawList->AddRectFilled(ImVec2(x0, mapPos.y), ImVec2(x1, mapPos.y + mapHeight), color);

        // Hover detection over the full block (node + data)
        const f32 fullX0 = mapPos.x + xCursor - nodeWidth;
        ImVec2 blockMin(fullX0, mapPos.y);
        if (ImVec2 blockMax(x1, mapPos.y + mapHeight); ImGui::IsMouseHoveringRect(blockMin, blockMax))
        {
            hoveredBlock = static_cast<s32>(i);
        }

        xCursor += blockWidth;
    }

    // Tooltip for hovered block
    if (hoveredBlock >= 0)
    {
        const auto& [address, size, isFree] = sBlocks[hoveredBlock];
        ImGui::BeginTooltip();
        ImGui::Text("Block %d: %s", hoveredBlock, isFree ? "FREE" : "ALLOCATED");
        ImGui::Text("Address: 0x%llX", static_cast<u64>(address));
        ImGui::Text("Size: 0x%X (%u bytes)", static_cast<u32>(size), static_cast<u32>(size));
        ImGui::EndTooltip();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // --- Block list ---
    ImGui::Text("Block List (%u blocks)", sNodeCount);

    if (ImGui::BeginTable("##blocks", 4,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                          ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable,
                          ImVec2(0, ImGui::GetContentRegionAvail().y)))
    {
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < sBlocks.size(); ++i)
        {
            const auto& [address, size, isFree] = sBlocks[i];
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
            ImGui::Text("0x%llX", static_cast<u64>(address));

            ImGui::TableNextColumn();
            ImGui::Text("0x%X (%u)", static_cast<u32>(size), static_cast<u32>(size));
        }

        ImGui::EndTable();
    }
}
