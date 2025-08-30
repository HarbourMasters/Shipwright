#ifdef ENABLE_REMOTE_CONTROL

#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/OTRGlobals.h"

/**
 * HANDSHAKE
 * 
 * Sent by the client to the server when it first connects to the server, sends over both the local room settings
 * in case the room needs to be created, along with the current client state
 */

void Anchor::SendPacket_Handshake() {
    nlohmann::json payload;
    payload["type"] = HANDSHAKE;
    payload["roomId"] = CVarGetString(CVAR_REMOTE_ANCHOR("RoomId"), "");
    payload["roomState"] = PrepRoomState();
    payload["clientState"] = PrepClientState();

    SendJsonToRemote(payload);
}

#endif // ENABLE_REMOTE_CONTROL
