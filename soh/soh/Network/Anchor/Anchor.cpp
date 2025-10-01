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

Anchor* Anchor::Instance = new Anchor();

//MARK: - Should be in BuildLocalClientState.cpp
AnchorClient Anchor::BuildLocalClientState() {
    AnchorClient self;
    self.clientId = getOwnClientId();
    self.name = CVarGetString(CVAR_REMOTE_ANCHOR("Name"), "");
    self.color = CVarGetColor24(CVAR_REMOTE_ANCHOR("Color"), { 100, 255, 100 });
    self.clientVersion = clientVersion;
    self.teamId = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
    self.online = true;
    self.self = true;

    if (IsSaveLoaded()) {
        self.seed = IS_RANDO ? Rando::Context::GetInstance()->GetSeed() : 0;
        self.isSaveLoaded = true;
        self.isGameComplete = gSaveContext.ship.stats.gameComplete;
        self.sceneNum = gPlayState->sceneNum;
        self.entranceIndex = gSaveContext.entranceIndex;
    } else {
        self.seed = 0;
        self.isSaveLoaded = false;
        self.isGameComplete = false;
        self.sceneNum = SCENE_ID_MAX;
        self.entranceIndex = 0x00;
    }

    return self;
}



// MARK: - Overrides

void Anchor::Enable() {
    Network::Enable(CVarGetString(CVAR_REMOTE_ANCHOR("Host"), "anchor.proxysaw.dev"),
                    CVarGetInteger(CVAR_REMOTE_ANCHOR("Port"), 43385));
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
        SendPacket_UpdateClientState();
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
    //printf("gPlayState: {}", gPlayState);
    //printf("Player: {}", GET_PLAYER(gPlayState));
    SPDLOG_INFO("fileNum: {}", gSaveContext.fileNum);
    SPDLOG_INFO("gameMode: {}", gSaveContext.gameMode);

    if (gPlayState == nullptr) {
        return false;
    }

    /*if (GET_PLAYER(gPlayState) == nullptr) {
        return false;
    }

    if (gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2) {
        return false;
    }

    if (gSaveContext.gameMode != GAMEMODE_NORMAL) {
        return false;
    }*/

    return true;
}

#endif