#include "hookDebugger.h"
#include "../game-interactor/GameInteractor.h"
#include "../../UIWidgets.hpp"
#include <string>
#include <version>

static std::unordered_map<const char*, std::unordered_map<HOOK_ID, RegisteringInfo>> hookData;

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
        3,
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit
    )) {
        ImGui::TableSetupColumn("Id");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Registration Info");
        //ImGui::TableSetupColumn("Stub");
        ImGui::TableHeadersRow();
        for (auto& [id, registeringInfo] : hookData[hookName]) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%d", id);
            ImGui::TableNextColumn();
            switch (registeringInfo.type) {
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
            ImGui::Text("%s(%d:%d) %s", registeringInfo.file, registeringInfo.line, registeringInfo.column, registeringInfo.function);

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
        }
        ImGui::EndTable();
    }
}

void HookDebuggerWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(1250, 850), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Hook Debugger", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

#ifdef __cpp_lib_source_location
    for (auto& [hookName, _] : hookData) {
        if (ImGui::TreeNode(hookName)) {
            DrawHookRegisteringInfos(hookName);
            ImGui::TreePop();
        }
    }
#else
    ImGui::TextColored(
        yellow,
        "The Hook Debugger is unavaliable because SoH was compiled "
        "without \"<source_location>\" support "
        "(\"__cpp_lib_source_location\" not defined in \"<version>\")."
    );
#endif

    ImGui::End();
}

void HookDebuggerWindow::UpdateElement() {
    hookData.clear();

    #define DEFINE_HOOK(name, _) hookData.insert({#name, GameInteractor::Instance->GetRegisteringInfos<GameInteractor::name>()});

    #include "../game-interactor/GameInteractor_HookTable.h"

    #undef DEFINE_HOOK
}