#include "AudioSoundFont.h"

namespace LUS {
void* AudioSoundFont::GetPointer() {
    return &soundFont;
}

size_t AudioSoundFont::GetPointerSize() {
    return sizeof(SoundFont);
}
} // namespace LUS
