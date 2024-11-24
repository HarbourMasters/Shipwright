#include "hookDebugger.h"
#include "../game-interactor/GameInteractor.h"
#include "../../UIWidgets.hpp"
#include <string>
#include <version>

static std::unordered_map<const char*, std::unordered_map<HOOK_ID, HookInfo>> hookData;

const ImVec4 grey = ImVec4(0.75, 0.75, 0.75, 1);
const ImVec4 yellow = ImVec4(1, 1, 0, 1);
const ImVec4 red = ImVec4(1, 0, 0, 1);

void DrawHookRegisteringInfos(const char* hookName) {
    if (hookData[hookName].size() == 0) {
        ImGui::TextColored(grey, "No hooks found");
        return;
    }

    if (ImGui::BeginTable(
        ("Table##" + std::string(hookName)).c_str(),
        4,
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit
    )) {
        ImGui::TableSetupColumn("Id");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Registration Info");
        //ImGui::TableSetupColumn("Stub");
        ImGui::TableSetupColumn("Number of Calls");
        ImGui::TableHeadersRow();
        for (auto& [id, hookInfo] : hookData[hookName]) {
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::Text("%d", id);

            ImGui::TableNextColumn();
            switch (hookInfo.registering.type) {
                case HOOK_TYPE_NORMAL:
                    ImGui::Text("Normal");
                    break;
                case HOOK_TYPE_ID:
                    ImGui::Text("Id");
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
                ImGui::Text("%s(%d:%d) %s", hookInfo.registering.file, hookInfo.registering.line, hookInfo.registering.column, hookInfo.registering.function);
            } else {
                ImGui::TextColored(yellow, "[Unavaliable]");
            }

            //TODO: not currently possible
            /*
            ImGui::TableNextColumn();

            ImGui::BeginDisabled();

            bool stubButtonPressed = ImGui::Button(("Stub##" + std::to_string(id)).c_str());
            UIWidgets::SetLastItemHoverText("Stub this hook.\nThis is not possible to automatically undo.");

            if (stubButtonPressed) {
                //stub
            }

            ImGui::EndDisabled();
            */

            ImGui::TableNextColumn();
            ImGui::Text("%d", hookInfo.calls);
        }
        ImGui::EndTable();
    }
}

void HookDebuggerWindow::DrawElement() {
#ifndef __cpp_lib_source_location
    ImGui::TextColored(
        yellow,
        "Some features of the Hook Debugger are unavaliable because SoH was compiled "
        "without \"<source_location>\" support "
        "(\"__cpp_lib_source_location\" not defined in \"<version>\")."
    );
#endif

    for (auto& [hookName, _] : hookData) {
        if (ImGui::TreeNode(hookName)) {
            DrawHookRegisteringInfos(hookName);
            ImGui::TreePop();
        }
    }
}

void HookDebuggerWindow::UpdateElement() {
    hookData.clear();

    #define DEFINE_HOOK(name, _) hookData.insert({#name, GameInteractor::Instance->GetHookData<GameInteractor::name>()});

    #include "../game-interactor/GameInteractor_HookTable.h"

    #undef DEFINE_HOOK
}