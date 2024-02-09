#include "SetRoomList.h"

RomFile* SetRoomList::GetPointer() {
    return rooms.data();
}

size_t SetRoomList::GetPointerSize() {
	return rooms.size() * sizeof(RomFile);
}
