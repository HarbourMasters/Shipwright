#include "SetCameraSettings.h"

namespace Ship {
void* SetCameraSettings::GetPointer() {
    return &settings;
}

size_t SetCameraSettings::GetPointerSize() {
	return sizeof(EchoSettings);
}
} // namespace Ship
