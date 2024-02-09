#include "AudioSoundFont.h"

SoundFont* AudioSoundFont::GetPointer() {
    return &soundFont;
}

size_t AudioSoundFont::GetPointerSize() {
    return sizeof(SoundFont);
}
