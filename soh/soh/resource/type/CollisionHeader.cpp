#include "CollisionHeader.h"

namespace LUS {
void* CollisionHeader::GetPointer() {
    return &collisionHeaderData;
}

size_t CollisionHeader::GetPointerSize() {
    return sizeof(collisionHeaderData);
}
} // namespace LUS