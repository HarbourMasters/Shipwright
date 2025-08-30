#ifdef ENABLE_REMOTE_CONTROL

#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"

/**
 * DISABLE_ANCHOR
 * 
 * No current use, potentially will be used for a future feature.
 */

void Anchor::HandlePacket_DisableAnchor(nlohmann::json payload) {
    Disable();
}

#endif // ENABLE_REMOTE_CONTROL
