#ifdef ENABLE_REMOTE_CONTROL

#include "Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/nametag.h"
#include "soh/SohGui/UIWidgets.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;
}

// MARK: - Overrides

void Anchor::Enable() {
    Network::Enable(CVarGetString(CVAR_REMOTE_ANCHOR("Host"), "anchor.proxysaw.dev"),
                    CVarGetInteger(CVAR_REMOTE_ANCHOR("Port"), 43383));
    ownClientId = CVarGetInteger(CVAR_REMOTE_ANCHOR("LastClientId"), 0);
    roomState.ownerClientId = 0;
}

void Anchor::Disable() {
    Network::Disable();

    clients.clear();
    RefreshClientActors();
}

void Anchor::OnConnected() {
    SendPacket_Handshake();
    RegisterHooks();

    if (IsSaveLoaded()) {
        SendPacket_RequestTeamState();
    }
}

void Anchor::OnDisconnected() {
    RegisterHooks();
}

void Anchor::SendJsonToRemote(nlohmann::json payload) {
    if (!isConnected) {
        return;
    }

    payload["clientId"] = ownClientId;
    if (!payload.contains("quiet")) {
        SPDLOG_INFO("[Anchor] Sending payload:\n{}", payload.dump());
    }
    Network::SendJsonToRemote(payload);
}

void Anchor::OnIncomingJson(nlohmann::json payload) {
    // If it doesn't contain a type, it's not a valid payload
    if (!payload.contains("type")) {
        return;
    }

    // If it's not a quiet payload, log it
    if (!payload.contains("quiet")) {
        SPDLOG_INFO("[Anchor] Received payload:\n{}", payload.dump());
    }

    std::string packetType = payload["type"].get<std::string>();

    // Ignore packets from mismatched clients, except for ALL_CLIENT_STATE or UPDATE_CLIENT_STATE
    if (packetType != ALL_CLIENT_STATE && packetType != UPDATE_CLIENT_STATE) {
        if (payload.contains("clientId")) {
            uint32_t clientId = payload["clientId"].get<uint32_t>();
            if (clients.contains(clientId) && clients[clientId].clientVersion != clientVersion) {
                return;
            }
        }
    }

    // packetType here is a string so we can't use a switch statement
    if (packetType == ALL_CLIENT_STATE)
        HandlePacket_AllClientState(payload);
    else if (packetType == DAMAGE_PLAYER)
        HandlePacket_DamagePlayer(payload);
    else if (packetType == DISABLE_ANCHOR)
        HandlePacket_DisableAnchor(payload);
    else if (packetType == ENTRANCE_DISCOVERED)
        HandlePacket_EntranceDiscovered(payload);
    else if (packetType == GAME_COMPLETE)
        HandlePacket_GameComplete(payload);
    else if (packetType == GIVE_ITEM)
        HandlePacket_GiveItem(payload);
    else if (packetType == PLAYER_SFX)
        HandlePacket_PlayerSfx(payload);
    else if (packetType == PLAYER_UPDATE)
        HandlePacket_PlayerUpdate(payload);
    else if (packetType == UPDATE_TEAM_STATE)
        HandlePacket_UpdateTeamState(payload);
    else if (packetType == REQUEST_TEAM_STATE)
        HandlePacket_RequestTeamState(payload);
    else if (packetType == REQUEST_TELEPORT)
        HandlePacket_RequestTeleport(payload);
    else if (packetType == SERVER_MESSAGE)
        HandlePacket_ServerMessage(payload);
    else if (packetType == SET_CHECK_STATUS)
        HandlePacket_SetCheckStatus(payload);
    else if (packetType == SET_FLAG)
        HandlePacket_SetFlag(payload);
    else if (packetType == TELEPORT_TO)
        HandlePacket_TeleportTo(payload);
    else if (packetType == UNSET_FLAG)
        HandlePacket_UnsetFlag(payload);
    else if (packetType == UPDATE_BEANS_COUNT)
        HandlePacket_UpdateBeansCount(payload);
    else if (packetType == UPDATE_CLIENT_STATE)
        HandlePacket_UpdateClientState(payload);
    else if (packetType == UPDATE_ROOM_STATE)
        HandlePacket_UpdateRoomState(payload);
    else if (packetType == UPDATE_DUNGEON_ITEMS)
        HandlePacket_UpdateDungeonItems(payload);
}

// Macros to let us easily register and unregister functions when the anchor is enabled/disabled
#define HOOK(hook, condition, body)                                                    \
    static HOOK_ID hook = 0;                                                           \
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::hook>(hook);          \
    hook = 0;                                                                          \
    if (condition) {                                                                   \
        hook = GameInteractor::Instance->RegisterGameHook<GameInteractor::hook>(body); \
    }

#define HOOK_FOR_ID(hook, condition, id, body)                                                  \
    static HOOK_ID hook = 0;                                                                    \
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::hook>(hook);              \
    hook = 0;                                                                                   \
    if (condition) {                                                                            \
        hook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::hook>(id, body); \
    }

void Anchor::RegisterHooks() {
    HOOK(OnSceneSpawnActors, isConnected, [&]() {
        SendPacket_UpdateClientState();

        if (IsSaveLoaded()) {
            RefreshClientActors();
        }
    });

    HOOK(OnPresentFileSelect, isConnected, [&]() { SendPacket_UpdateClientState(); });

    HOOK_FOR_ID(ShouldActorInit, isConnected, ACTOR_PLAYER, [&](void* actorRef, bool* should) {
        Actor* actor = (Actor*)actorRef;

        if (refreshingActors) {
            // By the time we get here, the actor was already added to the ACTORCAT_PLAYER list, so we need to move it
            Actor_ChangeCategory(gPlayState, &gPlayState->actorCtx, actor, ACTORCAT_NPC);
            actor->id = ACTOR_EN_OE2;
            actor->category = ACTORCAT_NPC;
            actor->init = DummyPlayer_Init;
            actor->update = DummyPlayer_Update;
            actor->draw = DummyPlayer_Draw;
            actor->destroy = DummyPlayer_Destroy;
        }
    });

    HOOK(OnPlayerUpdate, isConnected, [&]() {
        if (justLoadedSave) {
            justLoadedSave = false;
            SendPacket_RequestTeamState();
        }
        SendPacket_PlayerUpdate();
    });

    HOOK(OnPlayerSfx, isConnected, [&](u16 sfxId) { SendPacket_PlayerSfx(sfxId); });

    HOOK(OnLoadGame, isConnected, [&](s16 fileNum) { justLoadedSave = true; });

    HOOK(OnSaveFile, isConnected, [&](s16 fileNum, int sectionID) {
        if (sectionID == 0) {
            SendPacket_UpdateTeamState();
        }
    });

    HOOK(OnFlagSet, isConnected, [&](s16 flagType, s16 flag) { SendPacket_SetFlag(SCENE_ID_MAX, flagType, flag); });

    HOOK(OnFlagUnset, isConnected, [&](s16 flagType, s16 flag) { SendPacket_UnsetFlag(SCENE_ID_MAX, flagType, flag); });

    HOOK(OnSceneFlagSet, isConnected,
         [&](s16 sceneNum, s16 flagType, s16 flag) { SendPacket_SetFlag(sceneNum, flagType, flag); });

    HOOK(OnSceneFlagUnset, isConnected,
         [&](s16 sceneNum, s16 flagType, s16 flag) { SendPacket_UnsetFlag(sceneNum, flagType, flag); });

    HOOK(OnRandoSetCheckStatus, isConnected, [&](RandomizerCheck rc, RandomizerCheckStatus status) {
        if (!isHandlingUpdateTeamState) {
            SendPacket_SetCheckStatus(rc);
        }
    });

    HOOK(OnRandoSetIsSkipped, isConnected, [&](RandomizerCheck rc, bool isSkipped) {
        if (!isHandlingUpdateTeamState) {
            SendPacket_SetCheckStatus(rc);
        }
    });

    HOOK(OnRandoEntranceDiscovered, isConnected,
         [&](u16 entranceIndex, u8 isReversedEntrance) { SendPacket_EntranceDiscovered(entranceIndex); });

    HOOK_FOR_ID(OnBossDefeat, isConnected, ACTOR_BOSS_GANON2, [&](void* refActor) { SendPacket_GameComplete(); });

    HOOK(OnItemReceive, isConnected, [&](GetItemEntry itemEntry) {
        // Handle vanilla dungeon items a bit differently
        if (itemEntry.modIndex == MOD_NONE &&
            (itemEntry.itemId >= ITEM_KEY_BOSS && itemEntry.itemId <= ITEM_KEY_SMALL)) {
            SendPacket_UpdateDungeonItems();
            return;
        }

        SendPacket_GiveItem(itemEntry.tableId, itemEntry.getItemId);
    });

    HOOK(OnDungeonKeyUsed, isConnected, [&](uint16_t mapIndex) {
        // Handle vanilla dungeon items a bit differently
        SendPacket_UpdateDungeonItems();
    });
}

// MARK: - Misc/Helpers

// Kills all existing anchor actors and respawns them with the new client data
void Anchor::RefreshClientActors() {
    if (!IsSaveLoaded()) {
        return;
    }

    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].head;

    while (actor != NULL) {
        if (actor->id == ACTOR_EN_OE2 && actor->update == DummyPlayer_Update) {
            NameTag_RemoveAllForActor(actor);
            Actor_Kill(actor);
        }
        actor = actor->next;
    }

    actorIndexToClientId.clear();
    refreshingActors = true;
    for (auto& [clientId, client] : clients) {
        if (!client.online || client.self) {
            continue;
        }

        actorIndexToClientId.push_back(clientId);
        // We are using a hook `ShouldActorInit` to override the init/update/draw/destroy functions of the Player we
        // spawn We quickly store a mapping of "index" to clientId, then within the init function we use this to get the
        // clientId and store it on player->zTargetActiveTimer (unused s32 for the dummy) for convenience
        auto dummy = Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_PLAYER, client.posRot.pos.x,
                                 client.posRot.pos.y, client.posRot.pos.z, client.posRot.rot.x, client.posRot.rot.y,
                                 client.posRot.rot.z, actorIndexToClientId.size() - 1, false);
        client.player = (Player*)dummy;
    }
    refreshingActors = false;
}

bool Anchor::IsSaveLoaded() {
    if (gPlayState == nullptr) {
        return false;
    }

    if (GET_PLAYER(gPlayState) == nullptr) {
        return false;
    }

    if (gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2) {
        return false;
    }

    if (gSaveContext.gameMode != GAMEMODE_NORMAL) {
        return false;
    }

    return true;
}

// MARK: - UI

void Anchor::DrawMenu() {
    ImGui::PushID("Anchor");

    std::string host = CVarGetString(CVAR_REMOTE_ANCHOR("Host"), "anchor.proxysaw.dev");
    uint16_t port = CVarGetInteger(CVAR_REMOTE_ANCHOR("Port"), 43383);
    std::string anchorTeamId = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
    std::string anchorRoomId = CVarGetString(CVAR_REMOTE_ANCHOR("RoomId"), "");
    std::string anchorName = CVarGetString(CVAR_REMOTE_ANCHOR("Name"), "");
    bool isFormValid = !SohUtils::IsStringEmpty(host) && port > 1024 && port < 65535 &&
                       !SohUtils::IsStringEmpty(anchorRoomId) && !SohUtils::IsStringEmpty(anchorName);

    ImGui::SeparatorText("Anchor");
    // UIWidgets::Tooltip("Anchor Stuff");
    if (ImGui::IsItemClicked()) {
        // ImGui::SetClipboardText("https://github.com/garrettjoecox/anchor");
    }

    ImGui::BeginDisabled(isEnabled);
    ImGui::Text("Host & Port");
    if (UIWidgets::InputString("##Host", &host)) {
        CVarSetString(CVAR_REMOTE_ANCHOR("Host"), host.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetFontSize() * 5);
    if (ImGui::InputScalar("##Port", ImGuiDataType_U16, &port)) {
        CVarSetInteger(CVAR_REMOTE_ANCHOR("Port"), port);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }

    ImGui::Text("Tunic Color & Name");
    static Color_RGBA8 color = CVarGetColor(CVAR_REMOTE_ANCHOR("Color"), { 100, 255, 100, 255 });
    static ImVec4 colorVec = ImVec4(color.r / 255.0, color.g / 255.0, color.b / 255.0, 1);
    if (ImGui::ColorEdit3("##Color", (float*)&colorVec, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        color.r = colorVec.x * 255.0;
        color.g = colorVec.y * 255.0;
        color.b = colorVec.z * 255.0;

        CVarSetColor(CVAR_REMOTE_ANCHOR("Color"), color);
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::InputString("##Name", &anchorName)) {
        CVarSetString(CVAR_REMOTE_ANCHOR("Name"), anchorName.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::Text("Room ID");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::InputString("##RoomId", &anchorRoomId/*, isEnabled ? ImGuiInputTextFlags_Password : 0*/)) {
        CVarSetString(CVAR_REMOTE_ANCHOR("RoomId"), anchorRoomId.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::Text("Team ID (Items & Flags Shared)");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::InputString("##TeamId", &anchorTeamId)) {
        CVarSetString(CVAR_REMOTE_ANCHOR("TeamId"), anchorTeamId.c_str());
        Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    ImGui::EndDisabled();

    ImGui::Spacing();

    ImGui::BeginDisabled(!isFormValid);
    const char* buttonLabel = isEnabled ? "Disable" : "Enable";
    if (ImGui::Button(buttonLabel, ImVec2(-1.0f, 0.0f))) {
        if (isEnabled) {
            CVarClear(CVAR_REMOTE_ANCHOR("Enabled"));
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            Disable();
        } else {
            CVarSetInteger(CVAR_REMOTE_ANCHOR("Enabled"), 1);
            Ship::Context::GetInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            Enable();
        }
    }
    ImGui::EndDisabled();

    if (isEnabled) {
        ImGui::Spacing();
        if (isConnected) {
            ImGui::Text("Connected");

            if (roomState.ownerClientId == ownClientId) {
                if (ImGui::BeginMenu("Room Settings")) {

                    //PvP
                    ImGui::Text("PvP Mode:");
                    static const std::unordered_map<int32_t, const char*> pvpModes =
                    { 
                        { 0, "Off" },
                        { 1, "On" },
                        { 2, "On + Friendly Fire" }
                    };

                    UIWidgets::ComboboxOptions options;

                    options.DefaultIndex(0)
                        .Color(UIWidgets::Colors::LightBlue)
                        .Tooltip("Do you want to be able to fight your friends?");

                    if (UIWidgets::CVarCombobox("PVP Settings", CVAR_REMOTE_ANCHOR("RoomSettings.PvpMode"), pvpModes, options)) {
                        SendPacket_UpdateRoomState();
                    }


                    //Locations
                    ImGui::Text("Show Locations For:");
                    static const std::unordered_map<int32_t, const char*> showLocationsMap =
                    { 
                        { 0, "None" },
                        { 1, "Team Only" },
                        { 2, "All" }
                    };

                    options.DefaultIndex(1) //"Team Only"
                        .Color(UIWidgets::Colors::LightBlue)
                        .Tooltip("Who can see your locations on the map?");
                        

                    if (UIWidgets::CVarCombobox("Show Locations", CVAR_REMOTE_ANCHOR("RoomSettings.ShowLocationsMode"), showLocationsMap, options)) {
                        SendPacket_UpdateRoomState();
                    }

                    //Teleporting
                    ImGui::Text("Allow Teleporting To:");
                    static const std::unordered_map<int32_t, const char*> teleportModes =
                    { 
                        {0, "None" },
                        {1, "Team Only" },
                        {2, "All" }
                    };

                    options.DefaultIndex(1) //team only
                        .Color(UIWidgets::Colors::LightBlue)
                        .Tooltip("Do you want to be able to warp to your friends?");

                    if (UIWidgets::CVarCombobox("Teleport Modes", CVAR_REMOTE_ANCHOR("RoomSettings.TeleportMode"), teleportModes, options)) {
                        SendPacket_UpdateRoomState();
                    }
                    ImGui::EndMenu();
                }
            }

            if (ImGui::Button("Request Team State", ImVec2(ImGui::GetContentRegionAvail().x - 25.0f, 0.0f))) {
                SendPacket_RequestTeamState();
            }
            if (roomState.ownerClientId == ownClientId) {
                ImGui::SameLine();
                if (ImGui::Button(ICON_FA_TRASH)) {
                    SendPacket_ClearTeamState();
                }
                UIWidgets::Tooltip("Clear Team State");
            }
        } else {
            ImGui::Text("Connecting...");
        }
    }

    ImGui::PopID();
}

#endif