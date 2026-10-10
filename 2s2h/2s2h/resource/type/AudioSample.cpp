#include "AudioSample.h"

namespace SOH {
AudioSample::~AudioSample() {
    if (sample.book != nullptr && sample.book->book != nullptr) {
        delete[] sample.book->book;
    }
    if (sample.sampleAddr != nullptr) {
        delete[] sample.sampleAddr;
    }
}
Sample* AudioSample::GetPointer() {
    return &sample;
}

size_t AudioSample::GetPointerSize() {
    return sizeof(Sample);
}
} // namespace SOH