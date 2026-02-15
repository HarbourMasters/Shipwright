#include "hookDebugger.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/SohGui/UIWidgets.hpp"
#include "soh/OTRGlobals.h"
#include <string>
#include <version>

static std::map<const char*, std::map<HOOK_ID, HookInfo>*> hookData;

static bool hookOptCollapseAll; // A bool that will collapse all hook group once
static bool hookOptExpandAll;   // A bool that will expand all hook group once

const ImVec4 grey = ImVec4(0.75, 0.75, 0.75, 1);
const ImVec4 yellow = ImVec4(1, 1, 0, 1);
const ImVec4 red = ImVec4(1, 0, 0, 1);

void DrawHookRegisteringInfos(const char* hookName) {
    size_t numHooks = (*hookData[hookName]).size();

    if (numHooks == 0) {
        ImGui::TextColored(grey, "No hooks found");
        return;
    }

    ImGui::Text("Total Registered: %d", numHooks);

    if (ImGui::BeginTable(("Table##" + std::string(hookName)).c_str(), 4,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable |
                              ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Registration Info", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("# Calls", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        for (auto& [id, hookInfo] : (*hookData[hookName])) {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%d", id);

            ImGui::TableNextColumn();
            switch (hookInfo.registering.type) {
                case HOOK_TYPE_NORMAL:
                    ImGui::Text("Normal");
                    break;
                case HOOK_TYPE_ID:
                    ImGui::Text("ID");
                    break;
                case HOOK_TYPE_PTR:
                    ImGui::Text("Ptr");
                    break;
                case HOOK_TYPE_FILTER:
                    ImGui::Text("Filter");
                    break;
                default:
                    ImGui::TextColored(red, "[UNKNOWN]");
                    break;
            }

            ImGui::TableNextColumn();
            if (hookInfo.registering.valid) {
                // Replace the space after the return type of the parent function with a non-breaking space
                std::string parentFunction = std::string(hookInfo.registering.function);
                size_t pos = parentFunction.find_first_of(" ");
                if (pos != std::string::npos) {
                    parentFunction.replace(pos, 1, "\u00A0");
                }
                // Non breaking space to keep the arrow with the parent function
                ImGui::TextWrapped("%s(%d:%d) <-\u00A0%s", hookInfo.registering.file, hookInfo.registering.line,
                                   hookInfo.registering.column, parentFunction.c_str());
            } else {
                ImGui::TextColored(yellow, "[Unavailable]");
            }

            ImGui::TableNextColumn();
            ImGui::Text("%d", hookInfo.calls);
        }
        ImGui::EndTable();
    }
}

void HookDebuggerWindow::DrawElement() {
    bool collapseLogic = false;
    bool doingCollapseOrExpand = hookOptExpandAll || hookOptCollapseAll;

    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0));
#ifndef __cpp_lib_source_location
    ImGui::TextColored(yellow, "Some features of the Hook Debugger are unavailable because SoH was compiled "
                               "without \"<source_location>\" support "
                               "(\"__cpp_lib_source_location\" not defined in \"<version>\").");
#endif

    if (UIWidgets::Button("Expand All", UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(UIWidgets::Sizes::Inline))) {
        hookOptCollapseAll = false;
        hookOptExpandAll = true;
    }
    ImGui::SameLine();
    if (UIWidgets::Button("Collapse All",
                          UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(UIWidgets::Sizes::Inline))) {
        hookOptExpandAll = false;
        hookOptCollapseAll = true;
    }

    ImGui::PushFont(OTRGlobals::Instance->fontMonoLarger);

    for (auto& [hookName, _] : hookData) {
        if (doingCollapseOrExpand) {
            if (hookOptExpandAll) {
                collapseLogic = true;
            } else if (hookOptCollapseAll) {
                collapseLogic = false;
            }
            ImGui::SetNextItemOpen(collapseLogic, ImGuiCond_Always);
        }

        if (ImGui::TreeNode(hookName)) {
            DrawHookRegisteringInfos(hookName);
            ImGui::TreePop();
        }
    }

    ImGui::PopFont();
    ImGui::EndDisabled();

    if (doingCollapseOrExpand) {
        hookOptExpandAll = false;
        hookOptCollapseAll = false;
    }
}

void HookDebuggerWindow::InitElement() {
    hookOptExpandAll = false;
    hookOptCollapseAll = false;

#define DEFINE_HOOK(name, _) hookData.insert({ #name, GameInteractor::Instance->GetHookData<GameInteractor::name>() });

#include "../game-interactor/GameInteractor_HookTable.h"

#undef DEFINE_HOOK
}
