#include "SetRoomList.h"

namespace LUS {
RomFile* SetRoomList::GetPointer() {
    return rooms.data();
}

size_t SetRoomList::GetPointerSize() {
	return rooms.size() * sizeof(RomFile);
}
} // namespace LUS
