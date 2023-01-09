#include "SetWind.h"

namespace Ship {
void* SetLightingSettings::GetPointer() {
    return &settings;
}

size_t SetLightingSettings::GetPointerSize() {
	return sizeof(TimeSettings);
}
} // namespace Ship
