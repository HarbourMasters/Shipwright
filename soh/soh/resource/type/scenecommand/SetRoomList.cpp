#include "SetRoomList.h"

namespace LUS {
void* SetRoomList::GetPointer() {
    return rooms.data();
}

size_t SetRoomList::GetPointerSize() {
	return rooms.size() * sizeof(RomFile);
}
} // namespace LUS
