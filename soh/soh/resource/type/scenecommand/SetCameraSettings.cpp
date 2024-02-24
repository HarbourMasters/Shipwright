#include "SetCameraSettings.h"

namespace SOH {
CameraSettings* SetCameraSettings::GetPointer() {
    return &settings;
}

size_t SetCameraSettings::GetPointerSize() {
	return sizeof(CameraSettings);
}
} // namespace SOH
