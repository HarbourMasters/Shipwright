#include "SetSpecialObjects.h"

namespace Ship {
void* SetSpecialObjects::GetPointer() {
    return &specialObjects;
}

size_t SetSpecialObjects::GetPointerSize() {
	return sizeof(SpecialObjects);
}
} // namespace Ship
