#ifdef ENABLE_REMOTE_CONTROL

#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/OTRGlobals.h"

extern "C" {
#include "macros.h"
}

/**
 * UPDATE_BEANS_COUNT
 * 
 * Keeps the client's bean count in sync as they buy/use them
 */

void Anchor::SendPacket_UpdateBeansCount() {
    if (!IsSaveLoaded()) {
        return;
    }

    nlohmann::json payload;
    payload["type"] = UPDATE_BEANS_COUNT;
    payload["targetTeamId"] = CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default");
    payload["addToQueue"] = true;
    payload["amount"] = AMMO(ITEM_BEAN);
    payload["amountBought"] = BEANS_BOUGHT;

    SendJsonToRemote(payload);
}

void Anchor::HandlePacket_UpdateBeansCount(nlohmann::json payload) {
    if (!IsSaveLoaded()) {
        return;
    }

    AMMO(ITEM_BEAN) = payload["amount"].get<s8>();
    BEANS_BOUGHT = payload["amountBought"].get<s8>();
}

#endif // ENABLE_REMOTE_CONTROL
