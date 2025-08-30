#ifdef ENABLE_REMOTE_CONTROL

#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/OTRGlobals.h"

/**
 * REQUEST_TEAM_STATE
 *
 * Requests team state from the server, which will pass on the request to any connected teammates, or send the last known
 * state if no teammates are connected.
 *
 * This fires when loading into a file while Anchor is connected, or when Anchor is connected while a file is already
 * loaded
 *
 * Note: This can additionally be fired with a button in the menus to fix any desyncs that may have occurred in the save
 * state
 */

void Anchor::SendPacket_RequestTeamState() {
    if (!IsSaveLoaded()) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = REQUEST_TEAM_STATE;
    payload["targetTeamId"] = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_RequestTeamState(nlohmann::json payload) {
    if (!IsSaveLoaded()) {
        return;
    }

    SendPacket_UpdateTeamState();
}

#endif // ENABLE_REMOTE_CONTROL
