#include "SetSkyboxSettings.h"

namespace LUS {
SkyboxSettings* SetSkyboxSettings::GetPointer() {
    return &settings;
}

size_t SetSkyboxSettings::GetPointerSize() {
	return sizeof(SetSkyboxSettings);
}
} // namespace LUS
