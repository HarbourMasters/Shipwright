#include "AudioSoundFont.h"

namespace SOH {
SoundFont* AudioSoundFont::GetPointer() {
    return &soundFont;
}

size_t AudioSoundFont::GetPointerSize() {
    return sizeof(SoundFont);
}
} // namespace SOH
