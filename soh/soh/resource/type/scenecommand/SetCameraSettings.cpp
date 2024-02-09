#include "SetCameraSettings.h"

CameraSettings* SetCameraSettings::GetPointer() {
    return &settings;
}

size_t SetCameraSettings::GetPointerSize() {
	return sizeof(CameraSettings);
}
