#include "soh/Enhancements/Lang/Lang.h"
#include <map>
#include <string>

#include <libultraship/bridge/resourcebridge.h>
#include <fast/resource/ResourceType.h>
#include <fast/resource/type/DisplayList.h>
#include <ship/window/Window.h>
#include <ship/Context.h>

#include "soh/SohGui/UIWidgets.hpp"
#include "soh/SohGui/SohGui.hpp"
#include "soh/OTRGlobals.h"
#include "dlViewer.h"

extern "C" {
#include <z64.h>
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
    auto result = Ship::Context::GetRawInstance()->GetResourceManager()->GetArchiveManager()->ListFiles(
        "*" + std::string(searchString) + "*DL*");

    displayListSearchResults.clear();

    // Filter the file results even further as StormLib can only use wildcard searching
    for (size_t i = 0; i < result->size(); i++) {
        std::string val = result->at(i);
        if (val.ends_with("DL") || val.find("DL_") != std::string::npos) {
            displayListSearchResults.push_back(val);
        }
    }

    // Sort the final list
    std::sort(displayListSearchResults.begin(), displayListSearchResults.end(),
              [](const std::string& a, const std::string& b) {
                  return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), [](char c1, char c2) {
                      return std::tolower(c1) < std::tolower(c2);
                  });
              });
}

void DLViewerWindow::DrawElement() {
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
    // Debounce the search field as listing otr files is expensive
    UIWidgets::PushStyleInput(THEME_COLOR);
    ImGui::PushFont(OTRGlobals::Instance->GetGuiFont(OTRGlobals::Instance->fontMonoLarger));

    if (ImGui::InputText(
            Lang::UILabel("ui.text.enhancements.debugger.dlviewer.search_display_lists", "Search Display Lists"),
            searchString, ARRAY_COUNT(searchString))) {
        doSearch = true;
        searchDebounceFrames = 30;
    }
    UIWidgets::PopStyleInput();

    if (doSearch) {
        if (searchDebounceFrames == 0) {
            doSearch = false;
            PerformDisplayListSearch();
        }

        searchDebounceFrames--;
    }

    UIWidgets::PushStyleCombobox(THEME_COLOR);
    if (ImGui::BeginCombo(
            Lang::UILabel("ui.text.enhancements.debugger.dlviewer.active_display_list", "Active Display List"),
            activeDisplayList.c_str())) {
        for (size_t i = 0; i < displayListSearchResults.size(); i++) {
            if (ImGui::Selectable(displayListSearchResults[i].c_str())) {
                activeDisplayList = displayListSearchResults[i];
                break;
            }
        }
        ImGui::EndCombo();
    }
    UIWidgets::PopStyleCombobox();

    if (activeDisplayList == "") {
        ImGui::PopFont();
        ImGui::EndDisabled();
        return;
    }

    try {
        auto res = std::static_pointer_cast<Fast::DisplayList>(
            Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(activeDisplayList));

        if (res->GetInitData()->Type != static_cast<uint32_t>(Fast::ResourceType::DisplayList)) {
            ImGui::TextUnformatted(Lang::UIString(
                "ui.text.enhancements.debugger.dlviewer.resource_type_is_not_a_display_list_please_choose_another",
                "Resource type is not a Display List. Please choose another."));
            ImGui::PopFont();
            ImGui::EndDisabled();
            return;
        }

        ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.total_instruction_size_lu",
                                   "Total Instruction Size: %lu"),
                    res->Instructions.size());

        for (size_t i = 0; i < res->Instructions.size(); i++) {
            std::string id = "##CMD" + std::to_string(i);
            Gfx* gfx = (Gfx*)&res->Instructions[i];
            int cmd = static_cast<int>(gfx->words.w0 >> 24);
            if (cmdMap.find(cmd) == cmdMap.end())
                continue;

            std::string cmdLabel = cmdMap.at(cmd);

            ImGui::BeginGroup();
            ImGui::PushItemWidth(25.0f);
            ImGui::Text("%lu", i);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(175.0f);

            UIWidgets::PushStyleCombobox(THEME_COLOR);
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
            UIWidgets::PopStyleCombobox();

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
            if (cmd == G_RDPPIPESYNC) {}
            if (cmd == G_SETGRAYSCALE) {
                bool* state = (bool*)&gfx->words.w1;
                ImGui::SameLine();
                UIWidgets::PushStyleCheckbox(THEME_COLOR);
                if (ImGui::Checkbox(("state" + id).c_str(), state)) {
                    //
                }
                UIWidgets::PopStyleCheckbox();
            }
            if (cmd == G_SETTILE) {
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.fmt_u", "FMT: %u"),
                            _SHIFTR(gfx->words.w0, 21, 3));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.siz_u", "SIZ: %u"),
                            _SHIFTR(gfx->words.w0, 19, 2));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.line_u", "LINE: %u"),
                            _SHIFTR(gfx->words.w0, 9, 9));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.tmem_u", "TMEM: %u"),
                            _SHIFTR(gfx->words.w0, 0, 9));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.tile_u", "TILE: %u"),
                            _SHIFTR(gfx->words.w1, 24, 3));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.pal_u", "PAL: %u"),
                            _SHIFTR(gfx->words.w1, 20, 4));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.cmt_u", "CMT: %u"),
                            _SHIFTR(gfx->words.w1, 18, 2));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.maskt_u", "MASKT: %u"),
                            _SHIFTR(gfx->words.w1, 14, 4));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.shift_u", "SHIFT: %u"),
                            _SHIFTR(gfx->words.w1, 10, 4));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.cms_u", "CMS: %u"),
                            _SHIFTR(gfx->words.w1, 8, 2));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.masks_u", "MASKS: %u"),
                            _SHIFTR(gfx->words.w1, 4, 4));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.shifts_u", "SHIFTS: %u"),
                            _SHIFTR(gfx->words.w1, 0, 4));
            }
            if (cmd == G_SETTIMG) {
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.fmt_u", "FMT: %u"),
                            _SHIFTR(gfx->words.w0, 21, 3));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.siz_u", "SIZ: %u"),
                            _SHIFTR(gfx->words.w0, 19, 2));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.width_u", "WIDTH: %u"),
                            _SHIFTR(gfx->words.w0, 0, 10));
                ImGui::SameLine();
            }
            if (cmd == G_SETTIMG_OTR_HASH) {
                gfx++;
                uint64_t hash = ((uint64_t)gfx->words.w0 << 32) + (uint64_t)gfx->words.w1;
                const char* fileName = ResourceGetNameByCrc(hash);

                gfx--;
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.fmt_u", "FMT: %u"),
                            _SHIFTR(gfx->words.w0, 21, 3));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.siz_u", "SIZ: %u"),
                            _SHIFTR(gfx->words.w0, 19, 2));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.width_u", "WIDTH: %u"),
                            _SHIFTR(gfx->words.w0, 0, 10));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.texture_name_s", "Texture Name: %s"),
                            fileName);
            }
            if (cmd == G_SETTIMG_OTR_FILEPATH) {
                char* fileName = (char*)gfx->words.w1;
                gfx++;
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.fmt_u", "FMT: %u"),
                            _SHIFTR(gfx->words.w0, 21, 3));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.siz_u", "SIZ: %u"),
                            _SHIFTR(gfx->words.w0, 19, 2));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.width_u", "WIDTH: %u"),
                            _SHIFTR(gfx->words.w0, 0, 10));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.texture_name_s", "Texture Name: %s"),
                            fileName);
            }
            if (cmd == G_VTX) {
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.num_vtx_u", "Num VTX: %u"),
                            _SHIFTR(gfx->words.w0, 12, 8));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.offset_u", "Offset: %u"),
                            _SHIFTR(gfx->words.w0, 1, 7) - _SHIFTR(gfx->words.w0, 12, 8));
            }
            if (cmd == G_VTX_OTR_HASH) {
                gfx++;
                uint64_t hash = ((uint64_t)gfx->words.w0 << 32) + (uint64_t)gfx->words.w1;
                const char* fileName = ResourceGetNameByCrc(hash);

                gfx--;
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.num_vtx_u", "Num VTX: %u"),
                            _SHIFTR(gfx->words.w0, 12, 8));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.offset_u", "Offset: %u"),
                            _SHIFTR(gfx->words.w0, 1, 7) - _SHIFTR(gfx->words.w0, 12, 8));

                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.vertex_name_s", "Vertex Name: %s"),
                            fileName);
            }
            if (cmd == G_VTX_OTR_FILEPATH) {
                char* fileName = (char*)gfx->words.w1;

                gfx++;
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.num_vtx_u", "Num VTX: %u"),
                            _SHIFTR(gfx->words.w0, 12, 8));
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.offset_u", "Offset: %u"),
                            _SHIFTR(gfx->words.w0, 1, 7) - _SHIFTR(gfx->words.w0, 12, 8));

                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.vertex_name_s", "Vertex Name: %s"),
                            fileName);
            }
            if (cmd == G_DL) {}
            if (cmd == G_DL_OTR_HASH) {
                gfx++;
                uint64_t hash = ((uint64_t)gfx->words.w0 << 32) + (uint64_t)gfx->words.w1;
                const char* fileName = ResourceGetNameByCrc(hash);
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.dl_name_s", "DL Name: %s"),
                            fileName);
            }
            if (cmd == G_DL_OTR_FILEPATH) {
                char* fileName = (char*)gfx->words.w1;
                ImGui::SameLine();
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.dl_name_s", "DL Name: %s"),
                            fileName);
            }

            // Skip second half of instructions that are over 128-bit wide
            if (cmd == G_SETTIMG_OTR_HASH || cmd == G_DL_OTR_HASH || cmd == G_VTX_OTR_HASH || cmd == G_BRANCH_Z_OTR ||
                cmd == G_MARKER || cmd == G_MTX_OTR) {
                i++;
                ImGui::Text(Lang::UIFormat("ui.text.enhancements.debugger.dlviewer.lu_reserved_second_half_of_s",
                                           "%lu - Reserved - Second half of %s"),
                            i, cmdLabel.c_str());
            }
            ImGui::EndGroup();
        }
    } catch ([[maybe_unused]] const std::exception& e) {
        ImGui::TextUnformatted(Lang::UIString("ui.text.enhancements.debugger.dlviewer.error_displaying_dl_instructions",
                                              "Error displaying DL instructions."));
    }

    ImGui::PopFont();
    ImGui::EndDisabled();
}

void DLViewerWindow::InitElement() {
    PerformDisplayListSearch();
}
