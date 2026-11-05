#pragma once

#include "Notification.h"

namespace Notification {
// The complete sentence belongs to ui-lang; the item name is already resolved
// for the same UI locale. Keep the item highlighted without fixing word order.
Options MakePickup(const std::string& itemName, const char* itemIcon = nullptr);
} // namespace Notification
