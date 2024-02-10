#include "SetRoomList.h"

namespace SOH {
RomFile* SetRoomList::GetPointer() {
    return rooms.data();
}

size_t SetRoomList::GetPointerSize() {
	return rooms.size() * sizeof(RomFile);
}
} // namespace SOH
