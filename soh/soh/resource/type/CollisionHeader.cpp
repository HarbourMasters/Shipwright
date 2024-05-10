#include "CollisionHeader.h"

namespace SOH {
CollisionHeaderData* CollisionHeader::GetPointer() {
    return &collisionHeaderData;
}

size_t CollisionHeader::GetPointerSize() {
    return sizeof(collisionHeaderData);
}
} // namespace SOH