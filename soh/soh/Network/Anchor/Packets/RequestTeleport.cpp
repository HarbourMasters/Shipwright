#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"

/**
 * REQUEST_TELEPORT
 *
 * Because we don't have all the necessary information to directly teleport to a player, we emit a request,
 * in which they will respond with a TELEPORT_TO packet, with the necessary information.
 */

void Anchor::SendPacket_RequestTeleport(uint32_t clientId) {
    if (!CanTeleportTo(clientId)) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = REQUEST_TELEPORT;
    payload["targetClientId"] = clientId;

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_RequestTeleport(nlohmann::json payload) {
    if (!IsSaveLoaded()) {
        return;
    }

    uint32_t clientId = payload["clientId"].get<uint32_t>();
    SendPacket_TeleportTo(clientId);
}

// Reusable function to check if teleporting to a client is allowed
bool Anchor::CanTeleportTo(uint32_t clientId) {
    // Teleporting is disabled
    if (roomState.teleportMode == 0) {
        return false;
    }

    // You're not loaded into a save
    if (!IsSaveLoaded()) {
        return false;
    }

    // The client doesn't exist
    if (clients.find(clientId) == clients.end()) {
        return false;
    }

    AnchorClient& client = clients[clientId];

    // The client is yourself
    if (client.self) {
        return false;
    }

    // The client isn't online or loaded into a save
    if (!client.online || !client.isSaveLoaded) {
        return false;
    }

    // Teleporting to team only, but the client is not on your team
    std::string ownTeamId = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
    if (roomState.teleportMode == 1 && client.teamId != ownTeamId) {
        return false;
    }

    // Problematic scenes for teleporting
    if (client.sceneNum == SCENE_ID_MAX || client.sceneNum == SCENE_GROTTOS || client.sceneNum == SCENE_MARKET_DAY ||
        client.sceneNum == SCENE_MARKET_NIGHT || client.sceneNum == SCENE_MARKET_RUINS ||
        client.sceneNum == SCENE_MARKET_ENTRANCE_DAY || client.sceneNum == SCENE_MARKET_ENTRANCE_NIGHT ||
        client.sceneNum == SCENE_MARKET_ENTRANCE_RUINS || client.sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY ||
        client.sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT ||
        client.sceneNum == SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS || client.sceneNum == SCENE_BACK_ALLEY_DAY ||
        client.sceneNum == SCENE_BACK_ALLEY_NIGHT) {
        return false;
    }

    // Child can't teleport to Ganon's Castle exterior
    if (client.sceneNum == SCENE_OUTSIDE_GANONS_CASTLE && gSaveContext.linkAge == LINK_AGE_CHILD) {
        return false;
    }

    // Adult can't teleport to Hyrule Castle exterior
    if (client.sceneNum == SCENE_HYRULE_CASTLE && gSaveContext.linkAge == LINK_AGE_ADULT) {
        return false;
    }

    return true;
}
