#include "SetSoundSettings.h"

namespace SOH {
SoundSettings* SetSoundSettings::GetPointer() {
    return &settings;
}

size_t SetSoundSettings::GetPointerSize() {
	return sizeof(SoundSettings);
}
} // namespace SOH
