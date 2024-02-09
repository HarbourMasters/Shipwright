#include "SetSpecialObjects.h"

SpecialObjects* SetSpecialObjects::GetPointer() {
    return &specialObjects;
}

size_t SetSpecialObjects::GetPointerSize() {
	return sizeof(SpecialObjects);
}
