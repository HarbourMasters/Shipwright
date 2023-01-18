#include "SetSoundSettings.h"

namespace Ship {
void* SetSoundSettings::GetPointer() {
    return &settings;
}

size_t SetSoundSettings::GetPointerSize() {
	return sizeof(SoundSettings);
}
} // namespace Ship
