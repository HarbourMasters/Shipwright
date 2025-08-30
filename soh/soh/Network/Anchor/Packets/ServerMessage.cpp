#ifdef ENABLE_REMOTE_CONTROL

#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>
#include <libultraship/libultraship.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"

/**
 * SERVER_MESSAGE
 */

void Anchor::HandlePacket_ServerMessage(nlohmann::json payload) {
    Notification::Emit({
        .prefix = "Server:",
        .prefixColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
        .message = payload["message"].get<std::string>(),
    });
}

#endif // ENABLE_REMOTE_CONTROL
