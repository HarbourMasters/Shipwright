#include "Anchor.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;
}

void AnchorRoomWindow::Draw() {
    if (!IsVisible() || !Anchor::Instance->isConnected) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_WindowBg,
                          ImVec4(0, 0, 0, CVarGetFloat(CVAR_SETTING("Notifications.BgOpacity"), 0.5f)));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    auto vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(vp->ID);

    ImGui::Begin("Anchor Room", nullptr,
                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

    DrawElement();

    ImGui::End();

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
}

void AnchorRoomWindow::DrawElement() {
    bool isGlobalRoom = (std::string("soh-global") == CVarGetString(CVAR_REMOTE_ANCHOR("RoomId"), ""));

    if (isGlobalRoom) {
        u32 activeClients = 0;
        for (auto& [clientId, client] : Anchor::Instance->clients) {
            if (client.online) {
                activeClients++;
            }
        }
        ImGui::Text("Players Online: %d", activeClients);
        return;
    }

    // First build a list of teams
    std::set<std::string> teams;
    for (auto& [clientId, client] : Anchor::Instance->clients) {
        teams.insert(client.teamId);
    }

    for (auto& team : teams) {
        if (teams.size() > 1) {
            ImGui::SeparatorText(team.c_str());
        }
        bool isOwnTeam = team == CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
        for (auto& [clientId, client] : Anchor::Instance->clients) {
            if (client.teamId != team) {
                continue;
            }

            ImGui::PushID(clientId);

            if (client.clientId == Anchor::Instance->roomState.ownerClientId) {
                ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", ICON_FA_GAVEL);
                ImGui::SameLine();
            }

            if (client.self) {
                ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.8f, 1.0f), "%s", CVarGetString(CVAR_REMOTE_ANCHOR("Name"), ""));
            } else if (!client.online) {
                ImGui::TextColored(ImVec4(1, 1, 1, 0.3f), "%s - offline", client.name.c_str());
                ImGui::PopID();
                continue;
            } else {
                ImGui::Text("%s", client.name.c_str());
            }

            if (Anchor::Instance->roomState.showLocationsMode == 2 ||
                (Anchor::Instance->roomState.showLocationsMode == 1 && isOwnTeam)) {
                if ((client.self ? Anchor::Instance->IsSaveLoaded() : client.isSaveLoaded)) {
                    ImGui::SameLine();
                    ImGui::TextColored(
                        ImVec4(1, 1, 1, 0.5f), "- %s",
                        SohUtils::GetSceneName(client.self ? gPlayState->sceneNum : client.sceneNum).c_str());
                }
            }

            if (Anchor::Instance->CanTeleportTo(client.clientId)) {
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                if (ImGui::Button(ICON_FA_LOCATION_ARROW, ImVec2(20.0f, 20.0f))) {
                    Anchor::Instance->SendPacket_RequestTeleport(client.clientId);
                }
                ImGui::PopStyleVar();
            }

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
            uint32_t seed = IS_RANDO ? Rando::Context::GetInstance()->GetSeed() : 0;
            if (client.isSaveLoaded && Anchor::Instance->IsSaveLoaded() && client.seed != seed && client.online &&
                !client.self) {
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
            ImGui::PopID();
        }
    }
}
