#include "SetSkyboxSettings.h"

namespace LUS {
void* SetSkyboxSettings::GetPointer() {
    return &settings;
}

size_t SetSkyboxSettings::GetPointerSize() {
	return sizeof(SetSkyboxSettings);
}
} // namespace LUS
