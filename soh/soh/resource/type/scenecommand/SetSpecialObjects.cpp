#include "SetSpecialObjects.h"

namespace LUS {
void* SetSpecialObjects::GetPointer() {
    return &specialObjects;
}

size_t SetSpecialObjects::GetPointerSize() {
	return sizeof(SpecialObjects);
}
} // namespace LUS
