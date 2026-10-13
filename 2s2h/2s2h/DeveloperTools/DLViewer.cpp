#include "DLViewer.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/BenGui/BenGui.hpp"
#include <fast/resource/type/DisplayList.h>
#include <fast/resource/ResourceType.h>
#include "BenPort.h"
#include <libultraship/bridge/resourcebridge.h>

extern "C" {
#include <z64.h>
#include "z64math.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
#include "gfx.h"
#include "PR/gbi.h"
void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);
size_t ResourceMgr_GetPatchCountForDL(const char* path);
void ResourceMgr_ResetAllPatchesForDL(const char* path);
}

// Constants
constexpr float SEARCH_DEBOUNCE_TIME = 0.5f;
constexpr float COLOR_INPUT_PADDING = 12.0f;
constexpr float COLOR_PREVIEW_SIZE = 20.0f;
constexpr float UNDO_BUTTON_SIZE = 27.0f;
constexpr float IDX_COLUMN_WIDTH = 50.0f;
constexpr float PARAMS_COLUMN_WIDTH = 350.0f;

// Cached text widths (calculated in InitElement)
static float cachedInputWidth255 = 0.0f;
static float cachedMaxCmdWidth = 0.0f;
static float cachedActionsHeaderWidth = 0.0f;

char searchString[64] = "";
std::string activeDisplayList = "";
std::vector<std::string> displayListSearchResults;
int16_t searchDebounceFrames = -1;
bool doSearch = false;
bool displayListResultsPopulated = false;

struct DisplayListEntry {
    std::string name;
    std::string lowerName;
};
static std::vector<DisplayListEntry> allDisplayLists;
static bool displayListIndexBuilt = false;

// Extended command map with all GBI commands
std::unordered_map<int, std::string> cmdMap = {
    { G_SETPRIMCOLOR, "gsDPSetPrimColor" },
    { G_SETENVCOLOR, "gsDPSetEnvColor" },
    { G_SETBLENDCOLOR, "gsDPSetBlendColor" },
    { G_SETFOGCOLOR, "gsDPSetFogColor" },
    { G_RDPPIPESYNC, "gsDPPipeSync" },
    { G_RDPTILESYNC, "gsDPTileSync" },
    { G_RDPLOADSYNC, "gsDPLoadSync" },
    { G_RDPFULLSYNC, "gsDPFullSync" },
    { G_SETGRAYSCALE, "gsSPGrayscale" },
    { G_SETINTENSITY, "gsDPSetGrayscaleColor" },
    { G_LOADTLUT, "gsDPLoadTLUT" },
    { G_ENDDL, "gsSPEndDisplayList" },
    { G_TEXTURE, "gsSPTexture" },
    { G_SETTIMG, "gsDPSetTextureImage" },
    { G_SETTIMG_OTR_HASH, "gsDPSetTextureImage" },
    { G_SETTIMG_OTR_FILEPATH, "gsDPSetTextureImage" },
    { G_SETTILE, "gsDPSetTile" },
    { G_LOADTILE, "gsDPLoadTile" },
    { G_LOADBLOCK, "gsDPLoadBlock" },
    { G_SETTILESIZE, "gsDPSetTileSize" },
    { G_DL, "gsSPDisplayList" },
    { G_DL_OTR_FILEPATH, "gsSPDisplayList" },
    { G_DL_OTR_HASH, "gsSPDisplayList" },
    { G_MTX, "gsSPMatrix" },
    { G_MTX_OTR, "gsSPMatrix" },
    { G_POPMTX, "gsSPPopMatrix" },
    { G_VTX, "gsSPVertex" },
    { G_VTX_OTR_FILEPATH, "gsSPVertex" },
    { G_VTX_OTR_HASH, "gsSPVertex" },
    { G_GEOMETRYMODE, "gsSPSetGeometryMode" },
    { G_SETOTHERMODE_H, "gsSPSetOtherMode_H" },
    { G_SETOTHERMODE_L, "gsSPSetOtherMode_L" },
    { G_TRI1, "gsSP1Triangle" },
    { G_TRI1_OTR, "gsSP1Triangle" },
    { G_TRI2, "gsSP2Triangles" },
    { G_SETCOMBINE, "gsDPSetCombineLERP" },
    { G_CULLDL, "gsSPCullDisplayList" },
    { G_NOOP, "gsDPNoOp" },
    { G_SPNOOP, "gsSPNoOp" },
    { G_MARKER, "LUS Custom Marker" },
    { G_MOVEWORD, "gsMoveWd" },
    { G_MOVEMEM, "gsDma1p/gsDma2p" },
};

// Command category classification
enum class CommandCategory { Color, Texture, Geometry, DisplayList, Sync, Other, Unknown };

CommandCategory GetCommandCategory(int cmd) {
    switch (cmd) {
        case G_SETPRIMCOLOR:
        case G_SETENVCOLOR:
        case G_SETBLENDCOLOR:
        case G_SETFOGCOLOR:
        case G_SETINTENSITY:
            return CommandCategory::Color;
        case G_SETTIMG:
        case G_SETTIMG_OTR_HASH:
        case G_SETTIMG_OTR_FILEPATH:
        case G_SETTILE:
        case G_LOADTILE:
        case G_LOADBLOCK:
        case G_SETTILESIZE:
        case G_LOADTLUT:
        case G_TEXTURE:
            return CommandCategory::Texture;
        case G_VTX:
        case G_VTX_OTR_FILEPATH:
        case G_VTX_OTR_HASH:
        case G_TRI1:
        case G_TRI1_OTR:
        case G_TRI2:
        case G_MTX:
        case G_MTX_OTR:
        case G_POPMTX:
        case G_GEOMETRYMODE:
        case G_CULLDL:
            return CommandCategory::Geometry;
        case G_DL:
        case G_DL_OTR_FILEPATH:
        case G_DL_OTR_HASH:
        case G_ENDDL:
            return CommandCategory::DisplayList;
        case G_RDPPIPESYNC:
        case G_RDPTILESYNC:
        case G_RDPLOADSYNC:
        case G_RDPFULLSYNC:
            return CommandCategory::Sync;
        case G_SETGRAYSCALE:
        case G_SETOTHERMODE_H:
        case G_SETOTHERMODE_L:
        case G_SETCOMBINE:
        case G_NOOP:
        case G_SPNOOP:
        case G_MARKER:
        case G_MOVEWORD:
        case G_MOVEMEM:
            return CommandCategory::Other;
        default:
            return CommandCategory::Unknown;
    }
}

static std::string ToLowerASCII(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; });
    return lower;
}

static void BuildDisplayListIndex() {
    // Get all DL files using broad pattern (glob_match is case-sensitive, so we filter manually)
    auto result = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->ListFiles("*DL*");

    allDisplayLists.clear();
    allDisplayLists.reserve(result->size());

    // Filter the file results even further as StormLib can only use wildcard searching
    for (const auto& val : *result) {
        std::string valLower = ToLowerASCII(val);

        // Check if string ends with "dl" or contains "dl_" (case-insensitive)
        bool endsWithDL = valLower.size() >= 2 && valLower.compare(valLower.size() - 2, 2, "dl") == 0;
        bool containsDL_ = valLower.find("dl_") != std::string::npos;

        if (endsWithDL || containsDL_) {
            allDisplayLists.push_back({ val, std::move(valLower) }); // Keep original case for display
        }
    }

    // Sort the final list
    std::sort(allDisplayLists.begin(), allDisplayLists.end(),
              [](const DisplayListEntry& a, const DisplayListEntry& b) { return a.lowerName < b.lowerName; });

    displayListIndexBuilt = true;
}

void PerformDisplayListSearch() {
    if (!displayListIndexBuilt) {
        BuildDisplayListIndex();
    }

    displayListSearchResults.clear();

    // Filtering preserves the index's ordering, so the results need no further sorting
    std::string searchLower = ToLowerASCII(searchString);
    for (const auto& entry : allDisplayLists) {
        if (searchLower.empty() || entry.lowerName.find(searchLower) != std::string::npos) {
            displayListSearchResults.push_back(entry.name);
        }
    }

    displayListResultsPopulated = true;
}

void DrawDLSelector() {
    // Debounce the search field as listing otr files is expensive
    UIWidgets::PushStyleInput(THEME_COLOR);
    if (OTRGlobals::Instance->fontMonoLarger != nullptr) {
        ImGui::PushFont(OTRGlobals::Instance->fontMonoLarger);
    }

    if (ImGui::InputText("Search Display Lists", searchString, ARRAY_COUNT(searchString))) {
        doSearch = true;
        searchDebounceFrames = static_cast<int16_t>(ImGui::GetIO().Framerate * SEARCH_DEBOUNCE_TIME);
    }
    UIWidgets::PopStyleInput();

    if (doSearch) {
        if (searchDebounceFrames <= 0) {
            doSearch = false;
            searchDebounceFrames = -1;
            PerformDisplayListSearch();
        } else {
            searchDebounceFrames--;
        }
    }

    UIWidgets::PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo("Active Display List", activeDisplayList.c_str())) {
        if (!displayListResultsPopulated) {
            PerformDisplayListSearch();
        }
        for (size_t i = 0; i < displayListSearchResults.size(); i++) {
            if (ImGui::Selectable(displayListSearchResults[i].c_str())) {
                activeDisplayList = displayListSearchResults[i];
                break;
            }
        }
        ImGui::EndCombo();
    }
    UIWidgets::PopStyleCombobox();

    if (OTRGlobals::Instance->fontMonoLarger != nullptr) {
        ImGui::PopFont();
    }
}

void DrawViewOptions() {
    if (UIWidgets::Button("Options", UIWidgets::ButtonOptions{}.Size(UIWidgets::Sizes::Inline))) {
        ImGui::OpenPopup("dlViewerOptions");
    }

    if (ImGui::BeginPopup("dlViewerOptions")) {
        UIWidgets::CVarCheckbox("Show Command Index", "gDeveloperTools.DLViewer.ShowCommandIndex",
                                { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("Show Raw Hex Values", "gDeveloperTools.DLViewer.ShowCommandHex",
                                { .defaultValue = 0 });
        UIWidgets::CVarCheckbox("Show Metadata", "gDeveloperTools.DLViewer.ShowMetadata", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("Color Preview", "gDeveloperTools.DLViewer.ColorPreview", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("Expanded View", "gDeveloperTools.DLViewer.ExpandedView", { .defaultValue = 0 });
        ImGui::EndPopup();
    }
}

void DrawInstructionFilters() {
    if (UIWidgets::Button("Filters", UIWidgets::ButtonOptions{}.Size(UIWidgets::Sizes::Inline))) {
        ImGui::OpenPopup("dlViewerFilters");
    }

    if (ImGui::BeginPopup("dlViewerFilters")) {
        bool allChecked = CVarGetInteger("gDeveloperTools.DLViewer.Filter.Color", 1) &&
                          CVarGetInteger("gDeveloperTools.DLViewer.Filter.Texture", 1) &&
                          CVarGetInteger("gDeveloperTools.DLViewer.Filter.Geometry", 1) &&
                          CVarGetInteger("gDeveloperTools.DLViewer.Filter.DisplayList", 1) &&
                          CVarGetInteger("gDeveloperTools.DLViewer.Filter.Sync", 1) &&
                          CVarGetInteger("gDeveloperTools.DLViewer.Filter.Other", 1) &&
                          CVarGetInteger("gDeveloperTools.DLViewer.Filter.Unknown", 0);

        bool someChecked = CVarGetInteger("gDeveloperTools.DLViewer.Filter.Color", 0) ||
                           CVarGetInteger("gDeveloperTools.DLViewer.Filter.Texture", 0) ||
                           CVarGetInteger("gDeveloperTools.DLViewer.Filter.Geometry", 0) ||
                           CVarGetInteger("gDeveloperTools.DLViewer.Filter.DisplayList", 0) ||
                           CVarGetInteger("gDeveloperTools.DLViewer.Filter.Sync", 0) ||
                           CVarGetInteger("gDeveloperTools.DLViewer.Filter.Other", 0) ||
                           CVarGetInteger("gDeveloperTools.DLViewer.Filter.Unknown", 0);

        ImGuiContext* g = ImGui::GetCurrentContext();
        ImGuiItemFlags backup_item_flags = g->CurrentItemFlags;
        if (!allChecked && someChecked)
            g->CurrentItemFlags |= ImGuiItemFlags_MixedValue;
        bool allCheckedValue = allChecked;
        if (UIWidgets::Checkbox("All", &allCheckedValue)) {
            int value = allCheckedValue ? 1 : 0;
            CVarSetInteger("gDeveloperTools.DLViewer.Filter.Color", value);
            CVarSetInteger("gDeveloperTools.DLViewer.Filter.Texture", value);
            CVarSetInteger("gDeveloperTools.DLViewer.Filter.Geometry", value);
            CVarSetInteger("gDeveloperTools.DLViewer.Filter.DisplayList", value);
            CVarSetInteger("gDeveloperTools.DLViewer.Filter.Sync", value);
            CVarSetInteger("gDeveloperTools.DLViewer.Filter.Other", value);
            CVarSetInteger("gDeveloperTools.DLViewer.Filter.Unknown", value);
            Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        }
        g->CurrentItemFlags = backup_item_flags;

        UIWidgets::CVarCheckbox("Color Commands", "gDeveloperTools.DLViewer.Filter.Color", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("Texture Commands", "gDeveloperTools.DLViewer.Filter.Texture", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("Geometry Commands", "gDeveloperTools.DLViewer.Filter.Geometry", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("Display List Commands", "gDeveloperTools.DLViewer.Filter.DisplayList",
                                { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("Sync Commands", "gDeveloperTools.DLViewer.Filter.Sync", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("Other Commands", "gDeveloperTools.DLViewer.Filter.Other", { .defaultValue = 1 });
        UIWidgets::CVarCheckbox("Unknown Commands", "gDeveloperTools.DLViewer.Filter.Unknown", { .defaultValue = 0 });
        ImGui::EndPopup();
    }
}

bool ShouldShowCommand(int cmd) {
    CommandCategory category = GetCommandCategory(cmd);
    switch (category) {
        case CommandCategory::Color:
            return CVarGetInteger("gDeveloperTools.DLViewer.Filter.Color", 1) != 0;
        case CommandCategory::Texture:
            return CVarGetInteger("gDeveloperTools.DLViewer.Filter.Texture", 1) != 0;
        case CommandCategory::Geometry:
            return CVarGetInteger("gDeveloperTools.DLViewer.Filter.Geometry", 1) != 0;
        case CommandCategory::DisplayList:
            return CVarGetInteger("gDeveloperTools.DLViewer.Filter.DisplayList", 1) != 0;
        case CommandCategory::Sync:
            return CVarGetInteger("gDeveloperTools.DLViewer.Filter.Sync", 1) != 0;
        case CommandCategory::Other:
            return CVarGetInteger("gDeveloperTools.DLViewer.Filter.Other", 1) != 0;
        case CommandCategory::Unknown:
            return CVarGetInteger("gDeveloperTools.DLViewer.Filter.Unknown", 0) != 0;
        default:
            return true;
    }
}

void DrawColorEditor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a, const std::string& id, Gfx* gfx, size_t index) {
    // Use cached width if available, otherwise calculate
    float inputWidth = cachedInputWidth255;
    if (inputWidth == 0.0f) {
        float textWidth = ImGui::CalcTextSize("255").x;
        float framePadding = ImGui::GetStyle().FramePadding.x * 2.0f;
        inputWidth = textWidth + framePadding + COLOR_INPUT_PADDING;
        cachedInputWidth255 = inputWidth;
    }

    UIWidgets::PushStyleInput(THEME_COLOR);
    ImGui::PushItemWidth(inputWidth);
    bool changed = false;

    ImGui::AlignTextToFramePadding();

    // Helper lambda to draw input
    auto DrawInput = [&](const char* label, uint8_t& value, const char* letter) {
        char buf[4];
        snprintf(buf, sizeof(buf), "%u", value);

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsDecimal;
        if (ImGui::InputText(label, buf, sizeof(buf), flags)) {
            int newVal = atoi(buf);
            if (newVal >= 0 && newVal <= 255) {
                value = static_cast<uint8_t>(newVal);
                changed = true;
            }
        }

        ImGui::SameLine();
        ImGui::Text("%s", letter);
        ImGui::SameLine();
    };

    DrawInput(("##R" + id).c_str(), r, "R");
    DrawInput(("##G" + id).c_str(), g, "G");
    DrawInput(("##B" + id).c_str(), b, "B");
    DrawInput(("##A" + id).c_str(), a, "A");

    ImGui::PopItemWidth();
    UIWidgets::PopStyleInput();

    if (changed) {
        Gfx patchedGfx = *gfx;
        patchedGfx.words.w1 = _SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(a, 0, 8);
        std::string patchName = "CMD" + std::to_string(index) + "Debug";
        ResourceMgr_PatchGfxByName(activeDisplayList.c_str(), patchName.c_str(), index, patchedGfx);
    }

    // Color preview - vertically center with inputs
    if (CVarGetInteger("gDeveloperTools.DLViewer.ColorPreview", 1)) {
        ImGui::SameLine();
        // Get the line height to match input fields
        float lineHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f;
        float buttonSize = COLOR_PREVIEW_SIZE;
        float offsetY = (lineHeight - buttonSize) * 0.5f;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
        ImVec4 color = ImVec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        ImGui::ColorButton(("##preview" + id).c_str(), color,
                           ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker,
                           ImVec2(buttonSize, buttonSize));
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - offsetY);
    }
}

std::string GetCommandMetadata(Gfx* gfx, int cmd, size_t currentIndex, size_t totalInstructions) {
    std::string metadata;

    switch (cmd) {
        case G_SETTILE:
            metadata =
                fmt::format("FMT:{} SIZ:{} LINE:{} TMEM:{} TILE:{} PAL:{}", _SHIFTR(gfx->words.w0, 21, 3),
                            _SHIFTR(gfx->words.w0, 19, 2), _SHIFTR(gfx->words.w0, 9, 9), _SHIFTR(gfx->words.w0, 0, 9),
                            _SHIFTR(gfx->words.w1, 24, 3), _SHIFTR(gfx->words.w1, 20, 4));
            break;
        case G_SETTIMG:
            metadata = fmt::format("FMT:{} SIZ:{} WIDTH:{}", _SHIFTR(gfx->words.w0, 21, 3),
                                   _SHIFTR(gfx->words.w0, 19, 2), _SHIFTR(gfx->words.w0, 0, 10));
            break;
        case G_SETTIMG_OTR_HASH: {
            if (currentIndex + 1 < totalInstructions) {
                gfx++;
                uint64_t hash = ((uint64_t)gfx->words.w0 << 32) + (uint64_t)gfx->words.w1;
                const char* fileName = ResourceGetNameByCrc(hash);
                metadata = fmt::format("FMT:{} SIZ:{} WIDTH:{}", _SHIFTR(gfx->words.w0, 21, 3),
                                       _SHIFTR(gfx->words.w0, 19, 2), _SHIFTR(gfx->words.w0, 0, 10));
                gfx--;
                if (fileName) {
                    metadata += fmt::format(" | {}", fileName);
                }
            } else {
                metadata = "FMT:? SIZ:? WIDTH:? (incomplete)";
            }
            break;
        }
        case G_SETTIMG_OTR_FILEPATH: {
            if (currentIndex + 1 < totalInstructions) {
                char* fileName = (char*)gfx->words.w1;
                gfx++;
                metadata = fmt::format("FMT:{} SIZ:{} WIDTH:{}", _SHIFTR(gfx->words.w0, 21, 3),
                                       _SHIFTR(gfx->words.w0, 19, 2), _SHIFTR(gfx->words.w0, 0, 10));
                gfx--;
                if (fileName) {
                    metadata += fmt::format(" | {}", fileName);
                }
            } else {
                metadata = "FMT:? SIZ:? WIDTH:? (incomplete)";
            }
            break;
        }
        case G_VTX:
            metadata = fmt::format("Num:{} Offset:{}", _SHIFTR(gfx->words.w0, 12, 8),
                                   _SHIFTR(gfx->words.w0, 1, 7) - _SHIFTR(gfx->words.w0, 12, 8));
            break;
        case G_VTX_OTR_HASH: {
            uint32_t w0 = gfx->words.w0; // Save original w0
            if (currentIndex + 1 < totalInstructions) {
                gfx++;
                uint64_t hash = ((uint64_t)gfx->words.w0 << 32) + (uint64_t)gfx->words.w1;
                const char* fileName = ResourceGetNameByCrc(hash);
                metadata = fmt::format("Num:{} Offset:{}", _SHIFTR(w0, 12, 8), _SHIFTR(w0, 1, 7) - _SHIFTR(w0, 12, 8));
                gfx--;
                if (fileName) {
                    metadata += fmt::format(" | {}", fileName);
                }
            } else {
                metadata = fmt::format("Num:{} Offset:{} (incomplete)", _SHIFTR(w0, 12, 8),
                                       _SHIFTR(w0, 1, 7) - _SHIFTR(w0, 12, 8));
            }
            break;
        }
        case G_VTX_OTR_FILEPATH: {
            uint32_t w0 = gfx->words.w0; // Save original w0
            if (currentIndex + 1 < totalInstructions) {
                char* fileName = (char*)gfx->words.w1;
                gfx++;
                metadata = fmt::format("Num:{} Offset:{}", _SHIFTR(w0, 12, 8), _SHIFTR(w0, 1, 7) - _SHIFTR(w0, 12, 8));
                gfx--;
                if (fileName) {
                    metadata += fmt::format(" | {}", fileName);
                }
            } else {
                metadata = fmt::format("Num:{} Offset:{} (incomplete)", _SHIFTR(w0, 12, 8),
                                       _SHIFTR(w0, 1, 7) - _SHIFTR(w0, 12, 8));
            }
            break;
        }
        case G_DL_OTR_HASH: {
            if (currentIndex + 1 < totalInstructions) {
                gfx++;
                uint64_t hash = ((uint64_t)gfx->words.w0 << 32) + (uint64_t)gfx->words.w1;
                const char* fileName = ResourceGetNameByCrc(hash);
                gfx--;
                if (fileName) {
                    metadata = fileName;
                }
            } else {
                metadata = "(incomplete)";
            }
            break;
        }
        case G_DL_OTR_FILEPATH: {
            char* fileName = (char*)gfx->words.w1;
            if (fileName) {
                metadata = fileName;
            }
            break;
        }
        case G_SETGRAYSCALE: {
            bool state = gfx->words.w1 != 0;
            metadata = state ? "Enabled" : "Disabled";
            break;
        }
        default:
            break;
    }

    return metadata;
}

void DrawInstructionRow(Gfx* gfx, size_t index, const std::string& activeDL, size_t totalInstructions) {
    std::string id = "##CMD" + std::to_string(index);
    int cmd = gfx->words.w0 >> 24;

    if (!ShouldShowCommand(cmd)) {
        return;
    }

    // Cache command label lookup
    auto it = cmdMap.find(cmd);
    std::string cmdLabel = (it != cmdMap.end()) ? it->second : fmt::format("Unknown (0x{:02X})", cmd);

    ImGui::TableNextRow();

    // Calculate column indices dynamically
    int colIdx = 0;
    bool showIndex = CVarGetInteger("gDeveloperTools.DLViewer.ShowCommandIndex", 1) != 0;
    bool showMetadata = CVarGetInteger("gDeveloperTools.DLViewer.ShowMetadata", 1) != 0;

    // Index column
    if (showIndex) {
        ImGui::TableSetColumnIndex(colIdx++);
        ImGui::Text("%lu", index);
    }

    // Command column
    ImGui::TableSetColumnIndex(colIdx++);
    // Use cached width if available
    float maxCmdWidth = cachedMaxCmdWidth;
    if (maxCmdWidth == 0.0f) {
        float textWidth = ImGui::CalcTextSize("gsDPSetGrayscaleColor").x;
        float framePadding = ImGui::GetStyle().FramePadding.x * 2.0f;
        float arrowWidth = ImGui::GetFrameHeight(); // Width of dropdown arrow
        maxCmdWidth = textWidth + framePadding + arrowWidth;
        cachedMaxCmdWidth = maxCmdWidth;
    }
    ImGui::PushItemWidth(maxCmdWidth);
    UIWidgets::PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo(("##cmd" + id).c_str(), cmdLabel.c_str())) {
        std::string patchName = "CMD" + std::to_string(index) + "Debug";
        if (ImGui::Selectable("gsDPSetPrimColor") && cmd != G_SETPRIMCOLOR) {
            ResourceMgr_PatchGfxByName(activeDL.c_str(), patchName.c_str(), index,
                                       gsDPSetPrimColor(0, 0, 0, 0, 0, 255));
        }
        if (ImGui::Selectable("gsDPSetEnvColor") && cmd != G_SETENVCOLOR) {
            ResourceMgr_PatchGfxByName(activeDL.c_str(), patchName.c_str(), index, gsDPSetEnvColor(0, 0, 0, 255));
        }
        if (ImGui::Selectable("gsDPPipeSync") && cmd != G_RDPPIPESYNC) {
            ResourceMgr_PatchGfxByName(activeDL.c_str(), patchName.c_str(), index, gsDPPipeSync());
        }
        if (ImGui::Selectable("gsSPGrayscale") && cmd != G_SETGRAYSCALE) {
            ResourceMgr_PatchGfxByName(activeDL.c_str(), patchName.c_str(), index, gsSPGrayscale(true));
        }
        if (ImGui::Selectable("gsDPSetGrayscaleColor") && cmd != G_SETINTENSITY) {
            ResourceMgr_PatchGfxByName(activeDL.c_str(), patchName.c_str(), index, gsDPSetGrayscaleColor(0, 0, 0, 255));
        }
        if (ImGui::Selectable("gsSPNoOp") && cmd != G_NOOP) {
            ResourceMgr_PatchGfxByName(activeDL.c_str(), patchName.c_str(), index, gsSPNoOp());
        }
        ImGui::EndCombo();
    }
    UIWidgets::PopStyleCombobox();
    ImGui::PopItemWidth();

    // Parameters column
    ImGui::TableSetColumnIndex(colIdx++);
    ImGui::AlignTextToFramePadding();

    if (cmd == G_SETPRIMCOLOR || cmd == G_SETENVCOLOR || cmd == G_SETINTENSITY) {
        uint8_t r = _SHIFTR(gfx->words.w1, 24, 8);
        uint8_t g = _SHIFTR(gfx->words.w1, 16, 8);
        uint8_t b = _SHIFTR(gfx->words.w1, 8, 8);
        uint8_t a = _SHIFTR(gfx->words.w1, 0, 8);
        DrawColorEditor(r, g, b, a, id, gfx, index);
    } else if (cmd == G_SETGRAYSCALE) {
        bool state = gfx->words.w1 != 0;
        UIWidgets::PushStyleCheckbox(THEME_COLOR);
        if (ImGui::Checkbox(("##state" + id).c_str(), &state)) {
            Gfx patchedGfx = *gfx;
            patchedGfx.words.w1 = state ? 1 : 0;
            std::string patchName = "CMD" + std::to_string(index) + "Debug";
            ResourceMgr_PatchGfxByName(activeDL.c_str(), patchName.c_str(), index, patchedGfx);
        }
        UIWidgets::PopStyleCheckbox();
        ImGui::SameLine();
        ImGui::TextWrapped("State: %s", state ? "On" : "Off");
    } else if (CVarGetInteger("gDeveloperTools.DLViewer.ShowCommandHex", 0)) {
        ImGui::TextWrapped("w0: 0x%08X w1: 0x%08X", gfx->words.w0, gfx->words.w1);
    }

    // Metadata column
    if (showMetadata) {
        ImGui::TableSetColumnIndex(colIdx++);
        ImGui::AlignTextToFramePadding();
        std::string metadata = GetCommandMetadata(gfx, cmd, index, totalInstructions);
        if (!metadata.empty()) {
            if (OTRGlobals::Instance->fontMonoLarger != nullptr) {
                ImGui::PushFont(OTRGlobals::Instance->fontMonoLarger);
            }
            ImGui::TextWrapped("%s", metadata.c_str());
            if (OTRGlobals::Instance->fontMonoLarger != nullptr) {
                ImGui::PopFont();
            }
        }
    }

    // Actions column
    ImGui::TableSetColumnIndex(colIdx++);
    // Center the button in the column
    float columnWidth = ImGui::GetColumnWidth();
    float buttonSize = UNDO_BUTTON_SIZE;
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - buttonSize) * 0.5f);

    // Draw button with manually centered icon
    std::string patchName = "CMD" + std::to_string(index) + "Debug";
    if (UIWidgets::Button(("##undo" + id).c_str(),
                          UIWidgets::ButtonOptions{}.Size(ImVec2(buttonSize, buttonSize)).Tooltip("Remove patch"))) {
        ResourceMgr_UnpatchGfxByName(activeDL.c_str(), patchName.c_str());
    }

    // Manually center and draw the icon using the button's actual rect
    ImVec2 buttonMin = ImGui::GetItemRectMin();
    ImVec2 buttonMax = ImGui::GetItemRectMax();
    ImVec2 iconSize = ImGui::CalcTextSize(ICON_FA_UNDO);
    // Center horizontally and vertically
    // For vertical centering, use font size to account for baseline properly
    float fontSize = ImGui::GetFontSize();
    float buttonCenterX = buttonMin.x + (buttonMax.x - buttonMin.x) * 0.5f;
    float buttonCenterY = buttonMin.y + (buttonMax.y - buttonMin.y) * 0.5f;
    ImVec2 iconPos = ImVec2(buttonCenterX - iconSize.x * 0.5f, buttonCenterY - fontSize * 0.5f);
    ImGui::GetWindowDrawList()->AddText(iconPos, ImGui::GetColorU32(ImGuiCol_Text), ICON_FA_UNDO);
}

void DrawInstructionTable(std::shared_ptr<Fast::DisplayList> res) {
    size_t totalInstructions = res->Instructions.size();
    size_t visibleCount = 0;
    size_t patchCount = ResourceMgr_GetPatchCountForDL(activeDisplayList.c_str());

    // Count visible instructions
    for (size_t i = 0; i < totalInstructions; i++) {
        Gfx* gfx = (Gfx*)&res->Instructions[i];
        int cmd = gfx->words.w0 >> 24;
        if (ShouldShowCommand(cmd)) {
            visibleCount++;
        }
    }

    // Display stats - vertically center with button
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Total: %lu | Filtered: %lu | Patches: %lu", totalInstructions, visibleCount, patchCount);
    ImGui::SameLine();
    if (UIWidgets::Button("Reset All Patches",
                          UIWidgets::ButtonOptions{}.Color(UIWidgets::Colors::Red).Size(UIWidgets::Sizes::Inline))) {
        ResourceMgr_ResetAllPatchesForDL(activeDisplayList.c_str());
    }

    ImGui::Spacing();

    // Determine column count based on CVars
    int columnCount = 3; // Command, Parameters, Actions (always visible)
    if (CVarGetInteger("gDeveloperTools.DLViewer.ShowCommandIndex", 1))
        columnCount++;
    if (CVarGetInteger("gDeveloperTools.DLViewer.ShowMetadata", 1))
        columnCount++;

    int columnIndex = 0;
    if (ImGui::BeginTable("dlInstructions", columnCount,
                          ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter |
                              ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp)) {

        // Setup columns
        if (CVarGetInteger("gDeveloperTools.DLViewer.ShowCommandIndex", 1)) {
            ImGui::TableSetupColumn("Idx", ImGuiTableColumnFlags_WidthFixed, IDX_COLUMN_WIDTH);
        }
        // Use cached width if available
        float maxCmdWidth = cachedMaxCmdWidth;
        if (maxCmdWidth == 0.0f) {
            float textWidth = ImGui::CalcTextSize("gsDPSetGrayscaleColor").x;
            float framePadding = ImGui::GetStyle().FramePadding.x * 2.0f;
            float arrowWidth = ImGui::GetFrameHeight(); // Width of dropdown arrow
            maxCmdWidth = textWidth + framePadding + arrowWidth;
            cachedMaxCmdWidth = maxCmdWidth;
        }
        ImGui::TableSetupColumn("Command", ImGuiTableColumnFlags_WidthFixed, maxCmdWidth);
        ImGui::TableSetupColumn("Parameters", ImGuiTableColumnFlags_WidthFixed, PARAMS_COLUMN_WIDTH);
        if (CVarGetInteger("gDeveloperTools.DLViewer.ShowMetadata", 1)) {
            ImGui::TableSetupColumn("Metadata", ImGuiTableColumnFlags_WidthStretch);
        }
        // Auto-size Actions column to header text (use cached if available)
        float actionsHeaderWidth = cachedActionsHeaderWidth;
        if (actionsHeaderWidth == 0.0f) {
            actionsHeaderWidth = ImGui::CalcTextSize("Actions").x + ImGui::GetStyle().FramePadding.x * 2.0f;
            cachedActionsHeaderWidth = actionsHeaderWidth;
        }
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, actionsHeaderWidth);
        ImGui::TableHeadersRow();

        // Iterate through all instructions (ImGui tables handle virtualization internally)
        for (size_t i = 0; i < totalInstructions; i++) {
            Gfx* gfx = (Gfx*)&res->Instructions[i];
            int cmd = gfx->words.w0 >> 24;

            // Skip commands not in filter
            if (cmdMap.find(cmd) == cmdMap.end() && !CVarGetInteger("gDeveloperTools.DLViewer.Filter.Unknown", 0)) {
                continue;
            }

            // Skip if command category is filtered out
            if (!ShouldShowCommand(cmd)) {
                // Still need to skip second half of wide instructions even if filtered
                if (cmd == G_SETTIMG_OTR_HASH || cmd == G_DL_OTR_HASH || cmd == G_VTX_OTR_HASH ||
                    cmd == G_BRANCH_Z_OTR || cmd == G_MARKER || cmd == G_MTX_OTR) {
                    if (i + 1 < totalInstructions) {
                        i++; // Skip the second instruction
                    }
                }
                continue;
            }

            DrawInstructionRow(gfx, i, activeDisplayList, totalInstructions);

            // Skip second half of wide instructions
            if (cmd == G_SETTIMG_OTR_HASH || cmd == G_DL_OTR_HASH || cmd == G_VTX_OTR_HASH || cmd == G_BRANCH_Z_OTR ||
                cmd == G_MARKER || cmd == G_MTX_OTR) {
                i++;
                if (i < totalInstructions) {
                    ImGui::TableNextRow();
                    int colIdx = 0;
                    bool showIndex = CVarGetInteger("gDeveloperTools.DLViewer.ShowCommandIndex", 1) != 0;
                    if (showIndex) {
                        ImGui::TableSetColumnIndex(colIdx++);
                        ImGui::Text("%zu", i);
                    }
                    ImGui::TableSetColumnIndex(colIdx++);
                    ImGui::TextDisabled("(Reserved - Second half)");
                }
            }
        }

        ImGui::EndTable();
    }
}

void DrawTopToolbar() {
    DrawViewOptions();
    ImGui::SameLine();
    DrawInstructionFilters();
}

void DLViewerWindow::DrawElement() {
    // Cache text widths for performance (lazy initialization on first draw)
    if (cachedInputWidth255 == 0.0f) {
        float textWidth = ImGui::CalcTextSize("255").x;
        float framePadding = ImGui::GetStyle().FramePadding.x * 2.0f;
        cachedInputWidth255 = textWidth + framePadding + COLOR_INPUT_PADDING;
    }
    if (cachedMaxCmdWidth == 0.0f) {
        float textWidth = ImGui::CalcTextSize("gsDPSetGrayscaleColor").x;
        float framePadding = ImGui::GetStyle().FramePadding.x * 2.0f;
        float arrowWidth = ImGui::GetFrameHeight();
        cachedMaxCmdWidth = textWidth + framePadding + arrowWidth;
    }
    if (cachedActionsHeaderWidth == 0.0f) {
        cachedActionsHeaderWidth = ImGui::CalcTextSize("Actions").x + ImGui::GetStyle().FramePadding.x * 2.0f;
    }

    ImGui::BeginDisabled(CVarGetInteger("gDeveloperTools.DisableChanges", 0));

    if (OTRGlobals::Instance->fontMonoLarger != nullptr) {
        ImGui::PushFont(OTRGlobals::Instance->fontMonoLarger);
    }

    DrawDLSelector();

    if (activeDisplayList.empty()) {
        if (OTRGlobals::Instance->fontMonoLarger != nullptr) {
            ImGui::PopFont();
        }
        ImGui::EndDisabled();
        return;
    }

    try {
        auto res = std::static_pointer_cast<Fast::DisplayList>(
            Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(activeDisplayList));

        if (res->GetInitData()->Type != static_cast<uint32_t>(Fast::ResourceType::DisplayList)) {
            ImGui::Text("Resource type is not a Display List. Please choose another.");
            if (OTRGlobals::Instance->fontMonoLarger != nullptr) {
                ImGui::PopFont();
            }
            ImGui::EndDisabled();
            return;
        }

        DrawTopToolbar();
        ImGui::Spacing();
        DrawInstructionTable(res);

    } catch (const std::exception& e) { ImGui::Text("Error displaying DL instructions: %s", e.what()); }

    if (OTRGlobals::Instance->fontMonoLarger != nullptr) {
        ImGui::PopFont();
    }
    ImGui::EndDisabled();
}

void DLViewerWindow::InitElement() {
}
