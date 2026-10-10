#include "SetMinimapChests.h"

namespace SOH {

MinimapChestData* SetMinimapChests::GetPointer() {
    return chests.data();
}

size_t SetMinimapChests::GetPointerSize() {
    return sizeof(MinimapChestData);
}

} // namespace SOH