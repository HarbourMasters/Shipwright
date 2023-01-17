#include "SetLightingSettings.h"

namespace Ship {
void* SetLightingSettings::GetPointer() {
    return settings.data();
}

size_t SetLightingSettings::GetPointerSize() {
	return settings.size() * sizeof(EnvLightSettings);
}
} // namespace Ship
