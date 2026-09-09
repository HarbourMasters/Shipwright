#include "soh/Network/Anchor/Anchor.h"

/**
 * DISABLE_ANCHOR
 *
 * No current use, potentially will be used for a future feature.
 */

void Anchor::HandlePacket_DisableAnchor(nlohmann::json payload) {
    Disable();
}
