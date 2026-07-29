#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>

extern "C" {
#include "functions.h"
extern PlayState* gPlayState;
}

/**
 * PLAYER_SFX
 *
 * Sound effects, only sent to other clients in the same scene as the player
 */

void Anchor::SendPacket_PlayerSfx(u16 sfxId) {
    if (!IsSaveLoaded()) {
        return;
    }

    nlohmann::json payload;

    payload["type"] = PLAYER_SFX;
    payload["sfxId"] = sfxId;
    payload["quiet"] = true;

    for (auto& [clientId, client] : clients) {
        if (client.sceneNum == gPlayState->sceneNum && client.online && client.isSaveLoaded && !client.self) {
            payload["targetClientId"] = clientId;
            SendJsonToRemote(payload);
        }
    }
}

void Anchor::HandlePacket_PlayerSfx(nlohmann::json payload) {
    uint32_t clientId = payload.at("clientId").get<uint32_t>();
    u16 sfxId = payload.at("sfxId").get<u16>();

    if (!clients.contains(clientId) || !clients[clientId].player) {
        return;
    }

    Player_PlaySfx((Actor*)clients[clientId].player, sfxId);
}
