#include "SetObjectList.h"

int16_t* SetObjectList::GetPointer() {
    return objects.data();
}

size_t SetObjectList::GetPointerSize() {
	return objects.size() * sizeof(int16_t);
}
