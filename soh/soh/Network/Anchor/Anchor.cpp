#include "Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/nametag.h"
#include "soh/ObjectExtension/ObjectExtension.h"

extern "C" {
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;
}

// MARK: - Overrides

void Anchor::Enable() {
    Network::Enable(CVarGetString(CVAR_REMOTE_ANCHOR("Host"), "anchor.hm64.org"),
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
        SPDLOG_DEBUG("[Anchor] Sending payload:\n{}", payload.dump());
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
        SPDLOG_DEBUG("[Anchor] Received payload:\n{}", payload.dump());
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

    // Handle PLAYER_UPDATE packets immediately, no need to queue
    if (packetType == PLAYER_UPDATE) {
        HandlePacket_PlayerUpdate(payload);
        return;
    }

    // Queue all packets to be processed on the game thread
    std::lock_guard<std::mutex> lock(incomingPacketQueueMutex);
    incomingPacketQueue.push(payload);
}

void Anchor::ProcessIncomingPacketQueue() {
    std::lock_guard<std::mutex> lock(incomingPacketQueueMutex);

    while (!incomingPacketQueue.empty()) {
        nlohmann::json payload = incomingPacketQueue.front();
        incomingPacketQueue.pop();

        std::string packetType = payload["type"].get<std::string>();

        isProcessingIncomingPacket = true;

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
        else if (packetType == OCARINA_SFX)
            HandlePacket_OcarinaSfx(payload);
        else if (packetType == PLAYER_SFX)
            HandlePacket_PlayerSfx(payload);
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

        isProcessingIncomingPacket = false;
    }
}

// MARK: - Misc/Helpers

// Kills all existing anchor actors and respawns them with the new client data

struct DummyPlayerClientId {
    uint32_t clientId = 0;
};
static ObjectExtension::Register<DummyPlayerClientId> DummyPlayerClientIdRegister;

uint32_t Anchor::GetDummyPlayerClientId(const Actor* actor) {
    const DummyPlayerClientId* clientId = ObjectExtension::GetInstance().Get<DummyPlayerClientId>(actor);
    return clientId != nullptr ? clientId->clientId : 0;
}

void Anchor::SetDummyPlayerClientId(const Actor* actor, uint32_t clientId) {
    ObjectExtension::GetInstance().Set<DummyPlayerClientId>(actor, DummyPlayerClientId{ clientId });
}

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

    for (auto& [clientId, client] : clients) {
        if (!client.online || client.self) {
            continue;
        }

        spawningDummyPlayerForClientId = clientId;
        // We are using a hook `ShouldActorInit` to override the init/update/draw/destroy functions of the Player we
        // spawn We quickly store a mapping of "index" to clientId, then within the init function we use this to get the
        // clientId and store it on player->zTargetActiveTimer (unused s32 for the dummy) for convenience
        auto dummy =
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_PLAYER, client.posRot.pos.x, client.posRot.pos.y,
                        client.posRot.pos.z, client.posRot.rot.x, client.posRot.rot.y, client.posRot.rot.z, 0, false);
        client.player = (Player*)dummy;
    }
    spawningDummyPlayerForClientId = 0;
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
