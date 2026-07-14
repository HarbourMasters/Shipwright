#include "soh/Network/Anchor/Anchor.h"
#include <nlohmann/json.hpp>

/**
 * DISABLE_ANCHOR
 *
 * No current use, potentially will be used for a future feature.
 */

void Anchor::HandlePacket_DisableAnchor(nlohmann::json payload) {
    Disable();
}
