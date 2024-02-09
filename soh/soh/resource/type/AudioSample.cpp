#include "AudioSample.h"

Sample* AudioSample::GetPointer() {
    return &sample;
}

size_t AudioSample::GetPointerSize() {
    return sizeof(Sample);
}
