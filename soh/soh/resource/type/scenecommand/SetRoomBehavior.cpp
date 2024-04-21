#include "SetRoomBehavior.h"

namespace SOH {
RoomBehavior* SetRoomBehavior::GetPointer() {
    return &roomBehavior;
}

size_t SetRoomBehavior::GetPointerSize() {
	return sizeof(RoomBehavior);
}
} // namespace SOH
