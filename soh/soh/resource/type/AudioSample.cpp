#include "AudioSample.h"

namespace Ship {
void* AudioSample::GetPointer() {
    return &sample;
}

size_t AudioSample::GetPointerSize() {
    return sizeof(Sample);
}
} // namespace Ship