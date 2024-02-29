#include "SetObjectList.h"

namespace SOH {
int16_t* SetObjectList::GetPointer() {
    return objects.data();
}

size_t SetObjectList::GetPointerSize() {
	return objects.size() * sizeof(int16_t);
}
} // namespace SOH
