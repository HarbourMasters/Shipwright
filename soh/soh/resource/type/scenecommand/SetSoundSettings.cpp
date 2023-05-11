#include "SetSoundSettings.h"

namespace LUS {
void* SetSoundSettings::GetPointer() {
    return &settings;
}

size_t SetSoundSettings::GetPointerSize() {
	return sizeof(SoundSettings);
}
} // namespace LUS
