#include "SetLightingSettings.h"

EnvLightSettings* SetLightingSettings::GetPointer() {
    return settings.data();
}

size_t SetLightingSettings::GetPointerSize() {
	return settings.size() * sizeof(EnvLightSettings);
}
