#include "SetSkyboxSettings.h"

SkyboxSettings* SetSkyboxSettings::GetPointer() {
    return &settings;
}

size_t SetSkyboxSettings::GetPointerSize() {
	return sizeof(SetSkyboxSettings);
}
