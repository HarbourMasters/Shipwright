#include "SetObjectList.h"

namespace LUS {
int16_t* SetObjectList::GetPointer() {
    return objects.data();
}

size_t SetObjectList::GetPointerSize() {
	return objects.size() * sizeof(int16_t);
}
} // namespace LUS
