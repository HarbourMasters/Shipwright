#include "PickupNotification.h"
#include "soh/Enhancements/Lang/Lang.h"

namespace Notification {
Options MakePickup(const std::string& itemName, const char* itemIcon) {
    constexpr auto marker = "[[item]]";
    std::string sentence = Lang::TranslateOrFallback("ui.notifications.item_found", "You found [[item]]");
    auto position = sentence.find(marker);
    if (position == std::string::npos || sentence.find(marker, position + 8) != std::string::npos) {
        sentence = "You found [[item]]";
        position = sentence.find(marker);
    }
    return {
        .itemIcon = itemIcon,
        .prefix = sentence.substr(0, position),
        .prefixColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        .message = itemName,
        .messageColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
        .suffix = sentence.substr(position + 8),
        .suffixColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
        .joinText = true,
    };
}
} // namespace Notification
