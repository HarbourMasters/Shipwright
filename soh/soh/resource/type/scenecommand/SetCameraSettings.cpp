#include "SetCameraSettings.h"

namespace LUS {
CameraSettings* SetCameraSettings::GetPointer() {
    return &settings;
}

size_t SetCameraSettings::GetPointerSize() {
	return sizeof(CameraSettings);
}
} // namespace LUS
