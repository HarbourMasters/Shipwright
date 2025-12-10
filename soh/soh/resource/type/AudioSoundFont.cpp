#include "AudioSoundFont.h"

namespace SOH {

AudioSoundFont::~AudioSoundFont() {
    for (auto i : instrumentAddresses) {
        if (i != nullptr) {
            delete[] i->envelope;
            delete i;
        }
    }

    for (auto d : drumAddresses) {
        if (d != nullptr) {
            delete[] d->envelope;
            delete d;
        }
    }
}

SoundFont* AudioSoundFont::GetPointer() {
    return &soundFont;
}

size_t AudioSoundFont::GetPointerSize() {
    return sizeof(SoundFont);
}
} // namespace SOH