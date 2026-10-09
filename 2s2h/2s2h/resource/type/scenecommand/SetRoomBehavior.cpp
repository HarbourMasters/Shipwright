#include "SetRoomBehavior.h"

namespace SOH {
RoomBehaviorMM* SetRoomBehaviorMM::GetPointer() {
    return &roomBehavior;
}
size_t SetRoomBehaviorMM::GetPointerSize() {
    return sizeof(RoomBehaviorMM);
}
} // namespace SOH
