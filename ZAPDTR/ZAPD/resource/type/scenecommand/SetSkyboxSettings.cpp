#include "SetSkyboxSettings.h"

namespace Ship {
void* SetSkyboxSettings::GetPointer() {
    return &settings;
}

size_t SetSkyboxSettings::GetPointerSize() {
	return sizeof(SkyboxSettings);
}
} // namespace Ship
