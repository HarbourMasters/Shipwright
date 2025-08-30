#ifdef ENABLE_REMOTE_CONTROL

#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/Enhancements/randomizer/3drando/random.hpp"

const std::string gameCompleteMessages[] = {
    "killed Ganon",
    "saved Zelda",
    "proved their Courage",
    "collected the Triforce",
    "is the Hero of Time",
    "proved Mido wrong",
};

/**
 * GAME_COMPLETE
 */

void Anchor::SendPacket_GameComplete() {
    if (!IsSaveLoaded()) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = GAME_COMPLETE;

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_GameComplete(nlohmann::json payload) {
    uint32_t clientId = payload["clientId"].get<uint32_t>();
    if (!clients.contains(clientId)) {
        return;
    }

    AnchorClient& anchorClient = clients[clientId];
    anchorClient.isGameComplete = true;
    Notification::Emit({
        .prefix = anchorClient.name,
        .message = RandomElement(gameCompleteMessages),
    });
}

#endif // ENABLE_REMOTE_CONTROL
