#include "SetCameraSettings.h"

namespace LUS {
void* SetCameraSettings::GetPointer() {
    return &settings;
}

size_t SetCameraSettings::GetPointerSize() {
	return sizeof(CameraSettings);
}
} // namespace LUS
