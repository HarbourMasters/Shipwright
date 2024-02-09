#include "CollisionHeader.h"

CollisionHeaderData* CollisionHeader::GetPointer() {
    return &collisionHeaderData;
}

size_t CollisionHeader::GetPointerSize() {
    return sizeof(collisionHeaderData);
}
