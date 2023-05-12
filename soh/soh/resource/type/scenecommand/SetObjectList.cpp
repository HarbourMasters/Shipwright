#include "SetObjectList.h"

namespace LUS {
void* SetObjectList::GetPointer() {
    return objects.data();
}

size_t SetObjectList::GetPointerSize() {
	return objects.size() * sizeof(int16_t);
}
} // namespace LUS
