#ifdef ENABLE_REMOTE_CONTROL

#include "Anchor.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;
}

void AnchorRoomWindow::Draw() {
    if (!CVarGetInteger(CVAR_WINDOW("AnchorRoomEnabled"), 0)) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0.5f));
    ImGui::Begin("Anchor Room", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse);

    if (Anchor::Instance->clients.empty()) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "No clients connected");
    }

    ImGui::Text("Teams:");

    std::set<std::string> teams;

    SPDLOG_INFO("Clients size: {}", Anchor::Instance->clients.size());
    for (auto& [clientId, client] : Anchor::Instance->clients) {
        SPDLOG_INFO("Client: {}, self: {}, team: {}", client.name, client.self, client.teamId);

        teams.insert(client.teamId);
    }

    for (auto& team : teams) {
        if (teams.size() > 1) {
            ImGui::SeparatorText(team.c_str());
        }

        bool isOwnTeam = team == CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");

        for (auto& [clientId, client] : Anchor::Instance->clients) {
            SPDLOG_INFO("Client: {}, self: {}, team: {}", client.name, client.self, client.teamId);

            if (client.teamId != team) {
                continue;
            }

            bool isSelf = client.self;
            bool isOnline = client.online;

            if (client.clientId == Anchor::Instance->getOwnClientId()) {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", ICON_FA_GAVEL);
                ImGui::SameLine();
            }

            if (isSelf) {
                ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.8f, 1.0f), "%s", CVarGetString(CVAR_REMOTE_ANCHOR("Name"), ""));
            } else if (!isOnline) {
                ImGui::TextColored(ImVec4(1, 1, 1, 0.3f), "%s - offline", client.name.c_str());
                continue;
            } else {
                ImGui::Text("%s", client.name.c_str());
            }


            SPDLOG_INFO("Client {} IsSaveLoaded = {}", client.name.c_str(), client.isSaveLoaded);
            SPDLOG_INFO("Client {} sceneNum = {}", client.name.c_str(), client.sceneNum);


            //ImGui::Text("Scene ID: %d", client.sceneNum);
            ImGui::Text("Scene Name: %s", SohUtils::GetSceneName(client.sceneNum).c_str());

            // Scene display
            if (Anchor::Instance->roomState.showLocationsMode == 2 ||
                (Anchor::Instance->roomState.showLocationsMode == 1 && isOwnTeam)) {
                if ((client.self ? Anchor::Instance->IsSaveLoaded() : client.isSaveLoaded)) {
                    ImGui::SameLine();
                    ImGui::TextColored(
                        ImVec4(1, 1, 1, 0.5f), "- %s",
                        SohUtils::GetSceneName(client.self ? gPlayState->sceneNum : client.sceneNum).c_str());
                }
            }

            // No teleport button for yourself
            if (!client.self && Anchor::Instance->IsSaveLoaded() && client.isSaveLoaded &&
                (Anchor::Instance->roomState.teleportMode == 2 ||
                 (Anchor::Instance->roomState.teleportMode == 1 && isOwnTeam))) {
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                if (ImGui::Button(ICON_FA_LOCATION_ARROW, ImVec2(15.0f, 15.0f))) {
                    Anchor::Instance->SendPacket_RequestTeleport(client.clientId);
                }
                ImGui::PopStyleVar();
            }

            // Version mismatch
            if (client.clientVersion != Anchor::clientVersion) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), ICON_FA_EXCLAMATION_TRIANGLE);
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Incompatible version! Will not work together!");
                    ImGui::Text("Yours: %s", Anchor::clientVersion.c_str());
                    ImGui::Text("Theirs: %s", client.clientVersion.c_str());
                    ImGui::EndTooltip();
                }
            }

            // Seed mismatch
            uint32_t seed = IS_RANDO ? Rando::Context::GetInstance()->GetSeed() : 0;
            if (!client.self && client.isSaveLoaded && Anchor::Instance->IsSaveLoaded() && client.seed != seed &&
                client.online) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1, 0, 0, 1), ICON_FA_EXCLAMATION_TRIANGLE);
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Seed mismatch! Continuing will break things!");
                    ImGui::Text("Yours: %u", seed);
                    ImGui::Text("Theirs: %u", client.seed);
                    ImGui::EndTooltip();
                }
            }
        }

    }

    ImGui::End();
    ImGui::PopStyleColor();
}

#endif // ENABLE_REMOTE_CONTROL
