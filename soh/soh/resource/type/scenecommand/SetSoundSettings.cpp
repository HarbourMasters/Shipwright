#include "SetSoundSettings.h"

SoundSettings* SetSoundSettings::GetPointer() {
    return &settings;
}

size_t SetSoundSettings::GetPointerSize() {
	return sizeof(SoundSettings);
}
