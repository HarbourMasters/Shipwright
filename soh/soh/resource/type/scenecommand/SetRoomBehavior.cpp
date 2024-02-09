#include "SetRoomBehavior.h"

RoomBehavior* SetRoomBehavior::GetPointer() {
    return &roomBehavior;
}

size_t SetRoomBehavior::GetPointerSize() {
	return sizeof(RoomBehavior);
}
