#include "SetSpecialObjects.h"

namespace SOH {
SpecialObjects* SetSpecialObjects::GetPointer() {
    return &specialObjects;
}

size_t SetSpecialObjects::GetPointerSize() {
	return sizeof(SpecialObjects);
}
} // namespace SOH
