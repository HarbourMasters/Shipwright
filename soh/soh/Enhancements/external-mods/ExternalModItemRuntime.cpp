#include "ExternalModItemRuntime.h"

#include <algorithm>

extern "C" {
#include <z64.h>
}

namespace SOH {

bool ExternalModItemRuntime::IsHookshotItemId(int32_t itemId) {
    return itemId == ITEM_HOOKSHOT || itemId == ITEM_LONGSHOT;
}

void ExternalModItemRuntime::ApplySkyhookImpulse(void* playerPtr, float pullForce, float speed) {
    auto* player = static_cast<Player*>(playerPtr);
    if (player == nullptr) {
        return;
    }

    const float clampedPullForce = std::clamp(pullForce, 2.0f, 20.0f);
    const float clampedSpeed = std::clamp(speed, 2.0f, 35.0f);

    player->actor.velocity.y = std::max(player->actor.velocity.y, clampedPullForce);
    player->linearVelocity = std::max(player->linearVelocity, clampedSpeed);
}

} // namespace SOH
