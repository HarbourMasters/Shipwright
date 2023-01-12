#include "SetSkyboxModifier.h"

namespace Ship {
void* SetSkyboxModifier::GetPointer() {
    return &modifier;
}

size_t SetSkyboxModifier::GetPointerSize() {
	return sizeof(SkyboxModifier);
}
} // namespace Ship
