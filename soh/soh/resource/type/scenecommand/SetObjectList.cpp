#include "SetObjectList.h"

namespace Ship {
void* SetObjectList::GetPointer() {
    return objects.data();
}

size_t SetObjectList::GetPointerSize() {
	return objects.size() * sizeof(int16_t);
}
} // namespace Ship
