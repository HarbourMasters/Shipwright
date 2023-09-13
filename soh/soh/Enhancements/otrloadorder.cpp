#include "otrloadorder.h"

#include <Utils/StringHelper.h>


// Refresh the list of otrs based on what's in the directory
void ModLoadOrderWindow::RefreshOtrList(bool ignoreCvar) {
    mOtrList.clear();

    if (ignoreCvar || !strnlen(CVarGetString("gModLoadOrder", ""), 1)) {
        std::string patchesPath = LUS::Context::LocateFileAcrossAppDirs("mods", "soh");
        if (patchesPath.length() > 0 && std::filesystem::exists(patchesPath)) {
            if (std::filesystem::is_directory(patchesPath)) {
                for (const auto& p : std::filesystem::recursive_directory_iterator(patchesPath)) {
                    if (StringHelper::IEquals(p.path().extension().string(), ".otr")) {
                        mOtrList.push_back(p.path().generic_string());
                    }
                }
            }
        }
    } else {
        // todo: this efficently when we build out cvar array support
        std::stringstream otrListStringStream(CVarGetString("gModLoadOrder", ""));
        std::string otrString;
        while (getline(otrListStringStream, otrString, ',')) {
            mOtrList.push_back(otrString);
        }
    }

    mOtrListLoaded = true;
}

void ModLoadOrderWindow::SaveAndQuit() {
    // todo: this efficently when we build out cvar array support
    std::string otrListString = "";
    for (auto otrIt : mOtrList) {
        otrListString += otrIt;
        otrListString += ",";
    }
    CVarSetString("gModLoadOrder", otrListString.c_str());
    LUS::Context::GetInstance()->GetConsoleVariables()->Save();
    LUS::Context::GetInstance()->GetWindow()->Close();
}

void ModLoadOrderWindow::DrawElement() {
    if (!mOtrListLoaded) {
        RefreshOtrList(false);
    }

    ImGui::SetNextWindowSize(ImVec2(820, 630), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Mod .otr Load Order", &mIsVisible)) {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Refresh list")) {
        RefreshOtrList(true);
    }

    for (int n = 0; n < mOtrList.size(); n++)
    {
        const char* item = mOtrList[n].c_str();
        ImGui::Selectable(item);

        if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
        {
            int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
            if (n_next >= 0 && n_next < mOtrList.size())
            {
                std::string swapped = mOtrList[n];
                mOtrList[n] = mOtrList[n_next];
                mOtrList[n_next] = swapped;
                ImGui::ResetMouseDragDelta();
            }
        }
    }

    if (ImGui::Button("Save and Quit (like fully quit the game)")) {
        SaveAndQuit();
    }

    ImGui::End();
}