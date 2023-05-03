#include "actorViewer.h"
#include "../../util.h"
#include "../../UIWidgets.hpp"
#include <ImGuiImpl.h>
#include "ResourceManager.h"
#include "DisplayList.h"
#include "../../OTRGlobals.h"

#include <array>
#include <bit>
#include <map>
#include <string>
#include <libultraship/bridge.h>

extern "C" {
#include <z64.h>
#include "z64math.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;

char** ResourceMgr_ListFiles(const char* searchMask, int* resultSize);
}

char searchString[64] = "";
int displayListsSearchResultsCount;
char** displayListsSearchResults;
char* activeDisplayList = nullptr;

std::map<int, std::string> cmdMap = {
    { G_SETPRIMCOLOR, "gsDPSetPrimColor" },
    { G_SETENVCOLOR, "gsDPSetEnvColor" },
    { G_RDPPIPESYNC, "gsDPPipeSync" },
    { G_SETGRAYSCALE, "gsSPGrayscale" },
    { G_SETINTENSITY, "gsDPSetGrayscaleColor" },
    { G_LOADTLUT, "gsDPLoadTLUT" },
    { G_ENDDL, "gsSPEndDisplayList" },
};

void DrawDLViewer(bool& open) {
    if (!open) {
        CVarSetInteger("gDLViewerEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Display List Viewer", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    if (ImGui::InputText("Search Display Lists", searchString, ARRAY_COUNT(searchString))) {
        displayListsSearchResults = ResourceMgr_ListFiles(("*" + std::string(searchString) + "*DL").c_str(), &displayListsSearchResultsCount);
    }

    if (ImGui::BeginCombo("Active Display List", activeDisplayList)) {
        for (int i = 0; i < displayListsSearchResultsCount; i++) {
            if (ImGui::Selectable(displayListsSearchResults[i])) {
                activeDisplayList = displayListsSearchResults[i];
                break;
            }
        }
        ImGui::EndCombo();
    }
    if (activeDisplayList != nullptr) {
        auto res = std::static_pointer_cast<Ship::DisplayList>(OTRGlobals::Instance->context->GetResourceManager()->LoadResource(activeDisplayList));
        for (int i = 0; i < res->Instructions.size(); i++) {
            std::string id = "##CMD" + std::to_string(i);
            Gfx* gfx = (Gfx*)&res->Instructions[i];
            int cmd = gfx->words.w0 >> 24;
            if (cmdMap.find(cmd) == cmdMap.end()) continue;

            std::string cmdLabel = cmdMap.at(cmd);

            ImGui::BeginGroup();
            ImGui::PushItemWidth(25.0f);
            ImGui::Text("%d", i);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(150.0f);
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
            if (gfx->words.w0 >> 24 == G_SETPRIMCOLOR || gfx->words.w0 >> 24 == G_SETINTENSITY || gfx->words.w0 >> 24 == G_SETENVCOLOR) {
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
            if (gfx->words.w0 >> 24 == G_RDPPIPESYNC) {
            }
            if (gfx->words.w0 >> 24 == G_SETGRAYSCALE) {
                bool* state = (bool*)&gfx->words.w1;
                ImGui::SameLine();
                if (ImGui::Checkbox(("state" + id).c_str(), state)) {
                    // 
                }
            }
            ImGui::EndGroup();
        }
    }
    ImGui::End();
}

void InitDLViewer() {
    Ship::AddWindow("Developer Tools", "Display List Viewer", DrawDLViewer);

    displayListsSearchResults = ResourceMgr_ListFiles("*DL", &displayListsSearchResultsCount);
}
