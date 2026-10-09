#include "SetMinimapList.h"

namespace SOH {

MinimapListData* SetMinimapList::GetPointer() {
    return &list;
}

size_t SetMinimapList::GetPointerSize() {
    return sizeof(list);
}

} // namespace SOH