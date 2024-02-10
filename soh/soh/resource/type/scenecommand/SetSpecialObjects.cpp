#include "SetSpecialObjects.h"

namespace LUS {
SpecialObjects* SetSpecialObjects::GetPointer() {
    return &specialObjects;
}

size_t SetSpecialObjects::GetPointerSize() {
	return sizeof(SpecialObjects);
}
} // namespace LUS
