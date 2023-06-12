#include "AudioSoundFont.h"

namespace LUS {
SoundFont* AudioSoundFont::GetPointer() {
    return &soundFont;
}

size_t AudioSoundFont::GetPointerSize() {
    return sizeof(SoundFont);
}
} // namespace LUS
