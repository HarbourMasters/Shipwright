#include "AudioSoundFont.h"

namespace Ship {
void* AudioSoundFont::GetPointer() {
    return &soundFont;
}

size_t AudioSoundFont::GetPointerSize() {
    return sizeof(SoundFont);
}
} // namespace Ship
