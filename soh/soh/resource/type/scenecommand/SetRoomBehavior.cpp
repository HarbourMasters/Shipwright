#include "SetRoomBehavior.h"

namespace LUS {
void* SetRoomBehavior::GetPointer() {
    return &roomBehavior;
}

size_t SetRoomBehavior::GetPointerSize() {
	return sizeof(RoomBehavior);
}
} // namespace LUS
