#include "CollisionHeader.h"

namespace Ship {
void* CollisionHeader::GetPointer() {
    return &collisionHeaderData;
}

size_t CollisionHeader::GetPointerSize() {
    return sizeof(collisionHeaderData);
}
} // namespace Ship