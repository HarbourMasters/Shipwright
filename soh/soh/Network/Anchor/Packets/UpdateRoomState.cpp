#include "soh/Network/Anchor/Anchor.h"
#include "soh/Network/Anchor/JsonConversions.hpp"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/OTRGlobals.h"

extern "C" {
#include "variables.h"
extern PlayState* gPlayState;
}

/**
 * UPDATE_ROOM_STATE
 */

nlohmann::json Anchor::PrepRoomState() {
    nlohmann::json payload;
    payload["ownerClientId"] = ownClientId;
    bool isGlobalRoom = (std::string("soh-global") == CVarGetString(CVAR_REMOTE_ANCHOR("RoomId"), ""));

    if (isGlobalRoom) {
        // Global room uses hardcoded settings
        payload["pvpMode"] = 0;
        payload["showLocationsMode"] = 0;
        payload["teleportMode"] = 0;
        payload["syncItemsAndFlags"] = 0;
    } else {
        payload["pvpMode"] = CVarGetInteger(CVAR_REMOTE_ANCHOR("RoomSettings.PvpMode"), 1);
        payload["showLocationsMode"] = CVarGetInteger(CVAR_REMOTE_ANCHOR("RoomSettings.ShowLocationsMode"), 1);
        payload["teleportMode"] = CVarGetInteger(CVAR_REMOTE_ANCHOR("RoomSettings.TeleportMode"), 1);
        payload["syncItemsAndFlags"] = CVarGetInteger(CVAR_REMOTE_ANCHOR("RoomSettings.SyncItemsAndFlags"), 1);
    }

    return payload;
}

void Anchor::SendPacket_UpdateRoomState() {
    nlohmann::json payload;
    payload["type"] = UPDATE_ROOM_STATE;
    payload["state"] = PrepRoomState();

    Network::SendJsonToRemote(payload);
}

void Anchor::HandlePacket_UpdateRoomState(nlohmann::json payload) {
    if (!payload.contains("state")) {
        return;
    }

    roomState.ownerClientId = payload["state"]["ownerClientId"].get<uint32_t>();
    roomState.pvpMode = payload["state"]["pvpMode"].get<u8>();
    roomState.showLocationsMode = payload["state"]["showLocationsMode"].get<u8>();
    roomState.teleportMode = payload["state"]["teleportMode"].get<u8>();
    roomState.syncItemsAndFlags = payload["state"]["syncItemsAndFlags"].get<u8>();
}
