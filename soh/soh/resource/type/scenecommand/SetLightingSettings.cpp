#include "SetLightingSettings.h"

namespace SOH {
EnvLightSettings* SetLightingSettings::GetPointer() {
    return settings.data();
}

size_t SetLightingSettings::GetPointerSize() {
	return settings.size() * sizeof(EnvLightSettings);
}
} // namespace SOH
