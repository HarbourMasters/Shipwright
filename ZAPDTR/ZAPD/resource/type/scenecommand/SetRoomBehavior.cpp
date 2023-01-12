#include "SetRoomBehavior.h"

namespace Ship {
void* SetRoomBehavior::GetPointer() {
    return &roomBehavior;
}

size_t SetRoomBehavior::GetPointerSize() {
	return sizeof(RoomBehavior);
}
} // namespace Ship
