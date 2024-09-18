#include "actorViewer.h"
#include "../../util.h"
#include "../../UIWidgets.hpp"
#include "ResourceManager.h"
#include "DisplayList.h"
#include "../../OTRGlobals.h"

#include <array>
#include <bit>
#include <map>
#include <string>
#include <libultraship/libultraship.h>
#include "dlViewer.h"

extern "C" {
#include <z64.h>
#include "z64math.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
}

char searchString[64] = "";
std::string activeDisplayList = "";
std::vector<std::string> displayListSearchResults;
int16_t searchDebounceFrames = -1;
bool doSearch = false;

std::map<int, std::string> cmdMap = {
    { G_SETPRIMCOLOR, "gsDPSetPrimColor" },
    { G_SETENVCOLOR, "gsDPSetEnvColor" },
    { G_RDPPIPESYNC, "gsDPPipeSync" },
    { G_SETGRAYSCALE, "gsSPGrayscale" },
    { G_SETINTENSITY, "gsDPSetGrayscaleColor" },
    { G_LOADTLUT, "gsDPLoadTLUT" },
    { G_ENDDL, "gsSPEndDisplayList" },
    { G_TEXTURE, "gsSPTexture" },
    { G_SETTIMG, "gsDPSetTextureImage" },
    { G_SETTIMG_OTR_HASH, "gsDPSetTextureImage" },
    { G_SETTIMG_OTR_FILEPATH, "gsDPSetTextureImage" },
    { G_RDPTILESYNC, "gsDPTileSync" },
    { G_SETTILE, "gsDPSetTile" },
    { G_RDPLOADSYNC, "gsDPLoadSync" },
    { G_LOADBLOCK, "gsDPLoadBlock" },
    { G_SETTILESIZE, "gsDPSetTileSize" },
    { G_DL, "gsSPDisplayList" },
    { G_DL_OTR_FILEPATH, "gsSPDisplayList" },
    { G_DL_OTR_HASH, "gsSPDisplayList" },
    { G_MTX, "gsSPMatrix" },
    { G_MTX_OTR, "gsSPMatrix" },
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
};

void PerformDisplayListSearch() {
    auto result = Ship::Context::GetInstance()->GetResourceManager()->GetArchiveManager()->ListFiles("*" + std::string(searchString) + "*DL*");

    displayListSearchResults.clear();

    // Filter the file results even further as StormLib can only use wildcard searching
    for (size_t i = 0; i < result->size(); i++) {
        std::string val = result->at(i);
        if (val.ends_with("DL") || val.find("DL_") != std::string::npos) {
            displayListSearchResults.push_back(val);
        }
    }

    // Sort the final list
    std::sort(displayListSearchResults.begin(), displayListSearchResults.end(), [](const std::string& a, const std::string& b) {
        return std::lexicographical_compare(
            a.begin(), a.end(),
            b.begin(), b.end(),
            [](char c1, char c2) {
                return std::tolower(c1) < std::tolower(c2);
            }
        );
    });
}

void DLViewerWindow::DrawElement() {
    // Debounce the search field as listing otr files is expensive
    if (ImGui::InputText("Search Display Lists", searchString, ARRAY_COUNT(searchString))) {
        doSearch = true;
        searchDebounceFrames = 30;
    }

    if (doSearch) {
        if (searchDebounceFrames == 0) {
            doSearch = false;
            PerformDisplayListSearch();
        }

        searchDebounceFrames--;
    }

    if (ImGui::BeginCombo("Active Display List", activeDisplayList.c_str())) {
        for (size_t i = 0; i < displayListSearchResults.size(); i++) {
            if (ImGui::Selectable(displayListSearchResults[i].c_str())) {
                activeDisplayList = displayListSearchResults[i];
                break;
            }
        }
        ImGui::EndCombo();
    }

    if (activeDisplayList == "") {
        return;
    }

    try {
        auto res = std::static_pointer_cast<LUS::DisplayList>(Ship::Context::GetInstance()->GetResourceManager()->LoadResource(activeDisplayList));

        if (res->GetInitData()->Type != static_cast<uint32_t>(LUS::ResourceType::DisplayList)) {
            ImGui::Text("Resource type is not a Display List. Please choose another.");
            return;
        }

        ImGui::Text("Total Instruction Size: %lu", res->Instructions.size());

        for (size_t i = 0; i < res->Instructions.size(); i++) {
            std::string id = "##CMD" + std::to_string(i);
            Gfx* gfx = (Gfx*)&res->Instructions[i];
            int cmd = gfx->words.w0 >> 24;
            if (cmdMap.find(cmd) == cmdMap.end()) continue;

            std::string cmdLabel = cmdMap.at(cmd);

            ImGui::BeginGroup();
            ImGui::PushItemWidth(25.0f);
            ImGui::Text("%lu", i);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(175.0f);

            if (ImGui::BeginCombo(("CMD" + id).c_str(), cmdLabel.c_str())) {
                if (ImGui::Selectable("gsDPSetPrimColor") && cmd != G_SETPRIMCOLOR) {
                    *gfx = gsDPSetPrimColor(0, 0, 0, 0, 0, 255);
                }
                if (ImGui::Selectable("gsDPSetEnvColor")) {
                    *gfx = gsDPSetEnvColor(0, 0, 0, 255);
                }
                if (ImGui::Selectable("gsDPPipeSync")) {
                    *gfx = gsDPPipeSync();
                }
                if (ImGui::Selectable("gsSPGrayscale")) {
                    *gfx = gsSPGrayscale(true);
                }
                if (ImGui::Selectable("gsDPSetGrayscaleColor")) {
                    *gfx = gsDPSetGrayscaleColor(0, 0, 0, 255);
                }
                ImGui::EndCombo();
            }

            ImGui::PopItemWidth();

            if (cmd == G_SETPRIMCOLOR || cmd == G_SETINTENSITY || cmd == G_SETENVCOLOR) {
                uint8_t r = _SHIFTR(gfx->words.w1, 24, 8);
                uint8_t g = _SHIFTR(gfx->words.w1, 16, 8);
                uint8_t b = _SHIFTR(gfx->words.w1, 8, 8);
                uint8_t a = _SHIFTR(gfx->words.w1, 0, 8);
                ImGui::PushItemWidth(30.0f);
                ImGui::SameLine();
                if (ImGui::InputScalar(("r" + id).c_str(), ImGuiDataType_U8, &r)) {
                    gfx->words.w1 = _SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(a, 0, 8);
                }
                ImGui::SameLine();
                if (ImGui::InputScalar(("g" + id).c_str(), ImGuiDataType_U8, &g)) {
                    gfx->words.w1 = _SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(a, 0, 8);
                }
                ImGui::SameLine();
                if (ImGui::InputScalar(("b" + id).c_str(), ImGuiDataType_U8, &b)) {
                    gfx->words.w1 = _SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(a, 0, 8);
                }
                ImGui::SameLine();
                if (ImGui::InputScalar(("a" + id).c_str(), ImGuiDataType_U8, &a)) {
                    gfx->words.w1 = _SHIFTL(r, 24, 8) | _SHIFTL(g, 16, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(a, 0, 8);
                }
                ImGui::PopItemWidth();
            }
            if (cmd == G_RDPPIPESYNC) {
            }
            if (cmd == G_SETGRAYSCALE) {
                bool* state = (bool*)&gfx->words.w1;
                ImGui::SameLine();
                if (ImGui::Checkbox(("state" + id).c_str(), state)) {
                    // 
                }
            }
            if (cmd == G_SETTILE) {
                ImGui::SameLine();
                ImGui::Text("FMT: %u", _SHIFTR(gfx->words.w0, 21, 3));
                ImGui::SameLine();
                ImGui::Text("SIZ: %u", _SHIFTR(gfx->words.w0, 19, 2));
                ImGui::SameLine();
                ImGui::Text("LINE: %u", _SHIFTR(gfx->words.w0, 9, 9));
                ImGui::SameLine();
                ImGui::Text("TMEM: %u", _SHIFTR(gfx->words.w0, 0, 9));
                ImGui::SameLine();
                ImGui::Text("TILE: %u", _SHIFTR(gfx->words.w1, 24, 3));
                ImGui::SameLine();
                ImGui::Text("PAL: %u", _SHIFTR(gfx->words.w1, 20, 4));
                ImGui::SameLine();
                ImGui::Text("CMT: %u", _SHIFTR(gfx->words.w1, 18, 2));
                ImGui::SameLine();
                ImGui::Text("MASKT: %u", _SHIFTR(gfx->words.w1, 14, 4));
                ImGui::SameLine();
                ImGui::Text("SHIFT: %u", _SHIFTR(gfx->words.w1, 10, 4));
                ImGui::SameLine();
                ImGui::Text("CMS: %u", _SHIFTR(gfx->words.w1, 8, 2));
                ImGui::SameLine();
                ImGui::Text("MASKS: %u", _SHIFTR(gfx->words.w1, 4, 4));
                ImGui::SameLine();
                ImGui::Text("SHIFTS: %u", _SHIFTR(gfx->words.w1, 0, 4));
            }
            if (cmd == G_SETTIMG) {
                ImGui::SameLine();
                ImGui::Text("FMT: %u", _SHIFTR(gfx->words.w0, 21, 3));
                ImGui::SameLine();
                ImGui::Text("SIZ: %u", _SHIFTR(gfx->words.w0, 19, 2));
                ImGui::SameLine();
                ImGui::Text("WIDTH: %u", _SHIFTR(gfx->words.w0, 0, 10));
                ImGui::SameLine();
            }
            if (cmd == G_SETTIMG_OTR_HASH) {
                gfx++;
                uint64_t hash = ((uint64_t)gfx->words.w0 << 32) + (uint64_t)gfx->words.w1;
                const char* fileName = ResourceGetNameByCrc(hash);

                gfx--;
                ImGui::SameLine();
                ImGui::Text("FMT: %u", _SHIFTR(gfx->words.w0, 21, 3));
                ImGui::SameLine();
                ImGui::Text("SIZ: %u", _SHIFTR(gfx->words.w0, 19, 2));
                ImGui::SameLine();
                ImGui::Text("WIDTH: %u", _SHIFTR(gfx->words.w0, 0, 10));
                ImGui::SameLine();
                ImGui::Text("Texture Name: %s", fileName);
            }
            if (cmd == G_SETTIMG_OTR_FILEPATH) {
                char* fileName = (char*)gfx->words.w1;
                gfx++;
                ImGui::SameLine();
                ImGui::Text("FMT: %u", _SHIFTR(gfx->words.w0, 21, 3));
                ImGui::SameLine();
                ImGui::Text("SIZ: %u", _SHIFTR(gfx->words.w0, 19, 2));
                ImGui::SameLine();
                ImGui::Text("WIDTH: %u", _SHIFTR(gfx->words.w0, 0, 10));
                ImGui::SameLine();
                ImGui::Text("Texture Name: %s", fileName);
            }
            if (cmd == G_VTX) {
                ImGui::SameLine();
                ImGui::Text("Num VTX: %u", _SHIFTR(gfx->words.w0, 12, 8));
                ImGui::SameLine();
                ImGui::Text("Offset: %u", _SHIFTR(gfx->words.w0, 1, 7) - _SHIFTR(gfx->words.w0, 12, 8));
            }
            if (cmd == G_VTX_OTR_HASH) {
                gfx++;
                uint64_t hash = ((uint64_t)gfx->words.w0 << 32) + (uint64_t)gfx->words.w1;
                const char* fileName = ResourceGetNameByCrc(hash);

                gfx--;
                ImGui::SameLine();
                ImGui::Text("Num VTX: %u", _SHIFTR(gfx->words.w0, 12, 8));
                ImGui::SameLine();
                ImGui::Text("Offset: %u", _SHIFTR(gfx->words.w0, 1, 7) - _SHIFTR(gfx->words.w0, 12, 8));

                ImGui::SameLine();
                ImGui::Text("Vertex Name: %s", fileName);
            }
            if (cmd == G_VTX_OTR_FILEPATH) {
                char* fileName = (char*)gfx->words.w1;

                gfx++;
                ImGui::SameLine();
                ImGui::Text("Num VTX: %u", _SHIFTR(gfx->words.w0, 12, 8));
                ImGui::SameLine();
                ImGui::Text("Offset: %u", _SHIFTR(gfx->words.w0, 1, 7) - _SHIFTR(gfx->words.w0, 12, 8));

                ImGui::SameLine();
                ImGui::Text("Vertex Name: %s", fileName);
            }
            if (cmd == G_DL) {
            }
            if (cmd == G_DL_OTR_HASH) {
                gfx++;
                uint64_t hash = ((uint64_t)gfx->words.w0 << 32) + (uint64_t)gfx->words.w1;
                const char* fileName = ResourceGetNameByCrc(hash);
                ImGui::SameLine();
                ImGui::Text("DL Name: %s", fileName);
            }
            if (cmd == G_DL_OTR_FILEPATH) {
                char* fileName = (char*)gfx->words.w1;
                ImGui::SameLine();
                ImGui::Text("DL Name: %s", fileName);
            }

            // Skip second half of instructions that are over 128-bit wide
            if (cmd == G_SETTIMG_OTR_HASH || cmd == G_DL_OTR_HASH || cmd == G_VTX_OTR_HASH ||
                cmd == G_BRANCH_Z_OTR || cmd == G_MARKER || cmd == G_MTX_OTR) {
                i++;
                ImGui::Text("%lu - Reserved - Second half of %s", i, cmdLabel.c_str());
            }
            ImGui::EndGroup();
        }
    } catch (const std::exception& e) {
        ImGui::Text("Error displaying DL instructions.");
        return;
    }
}

void DLViewerWindow::InitElement() {
    PerformDisplayListSearch();
}
