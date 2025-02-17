#include "AboutWindow.h"
#include <imgui.h>
#include <soh/GameVersions.h>
#include "soh/ResourceManagerHelpers.h"

extern "C" {
#include "variables.h"
}

AboutWindow::~AboutWindow() {
    SPDLOG_TRACE("destruct about window");
}

void AboutWindow::InitElement() {
    mIsTaggedVersion = gGitCommitTag[0] != 0;

    strncpy(mGitCommitHashTruncated, (char*)gGitCommitHash, 7);
    mGitCommitHashTruncated[7] = 0;
}

void AboutWindow::Draw() {
    if (!IsVisible()) {
        return;
    }

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_AlwaysAutoResize |
                                   ImGuiWindowFlags_NoResize |
                                   ImGuiWindowFlags_NoDocking |
                                   ImGuiWindowFlags_NoScrollWithMouse |
                                   ImGuiWindowFlags_NoScrollbar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16 * ImGui::GetIO().FontGlobalScale, 8 * ImGui::GetIO().FontGlobalScale));

    if (!ImGui::Begin(GetName().c_str(), &mIsVisible, windowFlags)) {
        ImGui::End();
    } else {
        DrawElement();
        ImGui::End();
    }

    ImGui::PopStyleVar();

    // Sync up the IsVisible flag if it was changed by ImGui
    SyncVisibilityConsoleVariable();
}

const char* AboutWindow::GetGameVersionString(uint32_t index) {
    uint32_t gameVersion = ResourceMgr_GetGameVersion(index);
    switch (gameVersion) {
        case OOT_NTSC_US_10:
            return "NTSC-U 1.0";
        case OOT_NTSC_US_11:
            return "NTSC-U 1.1";
        case OOT_NTSC_US_12:
            return "NTSC-U 1.2";
        case OOT_PAL_10:
            return "PAL 1.0";
        case OOT_PAL_11:
            return "PAL 1.1";
        case OOT_PAL_GC:
            return "PAL GC";
        case OOT_PAL_MQ:
            return "PAL MQ";
        case OOT_PAL_GC_DBG1:
        case OOT_PAL_GC_DBG2:
            return "PAL GC-D";
        case OOT_PAL_GC_MQ_DBG:
            return "PAL MQ-D";
        case OOT_IQUE_CN:
            return "IQUE CN";
        case OOT_IQUE_TW:
            return "IQUE TW";
        default:
            return "UNKNOWN";
    }
}

void AboutWindow::DrawElement() {
    // The icon is already padded - adjust for that
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    cursorPos.x -= 16 * ImGui::GetIO().FontGlobalScale;
    ImGui::SetCursorScreenPos(cursorPos);

    ImGui::Image(Ship::Context::GetInstance()->GetWindow()->GetGui()->GetTextureByName("Game_Icon"), ImVec2(64.0f * ImGui::GetIO().FontGlobalScale, 64.0f * ImGui::GetIO().FontGlobalScale));

    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("Ship of Harkinian");
    if (mIsTaggedVersion) {
        ImGui::Text("%s", gBuildVersion);
    } else {
        ImGui::Text("%s", gGitBranch);
        ImGui::Text("%s", mGitCommitHashTruncated);
    }
    ImGui::EndGroup();

    ImGui::Dummy(ImVec2(0, 2 * ImGui::GetIO().FontGlobalScale));
    ImGui::Text("Game Archives:");
    for (uint32_t i = 0; i < ResourceMgr_GetNumGameVersions(); i++) {
        ImGui::BulletText(GetGameVersionString(i));
    }
}
