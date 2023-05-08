#include "SetSkyboxModifier.h"

namespace LUS {
void* SetSkyboxModifier::GetPointer() {
    return &modifier;
}

size_t SetSkyboxModifier::GetPointerSize() {
	return sizeof(SkyboxModifier);
}
} // namespace LUS
