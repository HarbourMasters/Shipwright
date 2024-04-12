#include "SetSkyboxSettings.h"

namespace SOH {
SkyboxSettings* SetSkyboxSettings::GetPointer() {
    return &settings;
}

size_t SetSkyboxSettings::GetPointerSize() {
	return sizeof(SetSkyboxSettings);
}
} // namespace SOH
