#include "soh/Network/Anchor/Anchor.h"
#include "soh/Network/Anchor/JsonConversions.hpp"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
extern f32 D_80130F28;
}

/**
 * OCARINA_SFX
 *
 * Ocarina effects, only sent to other clients in the same scene as the player
 */

void Anchor::SendPacket_OcarinaSfx(uint8_t note, float modulator, int8_t bend) {
    if (!IsSaveLoaded()) {
        return;
    }

    nlohmann::json payload;

    payload["type"] = OCARINA_SFX;
    payload["note"] = note;
    payload["modulator"] = modulator;
    payload["bend"] = bend;
    payload["quiet"] = true;

    for (auto& [clientId, client] : clients) {
        if (client.sceneNum == gPlayState->sceneNum && client.online && client.isSaveLoaded && !client.self) {
            payload["targetClientId"] = clientId;
            SendJsonToRemote(payload);
        }
    }
}

void Anchor::HandlePacket_OcarinaSfx(nlohmann::json payload) {
    uint32_t clientId = payload["clientId"].get<uint32_t>();
    uint8_t note = payload["note"].get<uint8_t>();
    float modulator = payload["modulator"].get<float>();
    int8_t bend = payload["bend"].get<int8_t>();

    if (!clients.contains(clientId) || !clients[clientId].player) {
        return;
    }

    auto& client = clients[clientId];
    client.ocarinaModulator = modulator;
    client.ocarinaBend = bend;

    if ((note != 0xFF) && (client.ocarinaNote != note)) {
        Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD07, client.ocarinaBend - 1);
        Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD05, note);
        Audio_PlaySoundGeneral(NA_SE_OC_OCARINA, &client.player->actor.projectedPos, 4, &client.ocarinaModulator,
                               &D_80130F28, &gSfxDefaultReverb);
    } else if ((client.ocarinaNote != 0xFF) && (note == 0xFF)) {
        Audio_StopSfxById(NA_SE_OC_OCARINA);
    }

    client.ocarinaNote = note;
}
