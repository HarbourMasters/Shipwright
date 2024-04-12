#include "SetCollisionHeader.h"

namespace SOH {
CollisionHeaderData* SetCollisionHeader::GetPointer() {
    if (collisionHeader == nullptr) {
        return nullptr;
    }
    return collisionHeader->GetPointer();
}

size_t SetCollisionHeader::GetPointerSize() {
    if (collisionHeader == nullptr) {
        return 0;
    }
	return collisionHeader->GetPointerSize();
}
} // namespace SOH
