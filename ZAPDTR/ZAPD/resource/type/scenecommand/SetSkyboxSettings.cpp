#include "SetSkyboxModifier.h"

namespace Ship {
void* SetSkyboxModifier::GetPointer() {
    return &settings;
}

size_t SetSkyboxModifier::GetPointerSize() {
	return sizeof(SkyboxSettings);
}
} // namespace Ship
