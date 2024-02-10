#include "AudioSample.h"

namespace LUS {
Sample* AudioSample::GetPointer() {
    return &sample;
}

size_t AudioSample::GetPointerSize() {
    return sizeof(Sample);
}
} // namespace LUS