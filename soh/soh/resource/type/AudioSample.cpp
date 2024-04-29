#include "AudioSample.h"

namespace SOH {
Sample* AudioSample::GetPointer() {
    return &sample;
}

size_t AudioSample::GetPointerSize() {
    return sizeof(Sample);
}
} // namespace SOH