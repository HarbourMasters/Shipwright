#include "SetExitList.h"

namespace Ship {
void* SetExitList::GetPointer() {
    return rooms.data();
}

size_t SetExitList::GetPointerSize() {
	return rooms.size() * sizeof(RomFile);
}
} // namespace Ship
