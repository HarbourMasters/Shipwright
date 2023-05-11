#include "SetCollisionHeader.h"

namespace LUS {
void* SetCollisionHeader::GetPointer() {
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
} // namespace LUS
