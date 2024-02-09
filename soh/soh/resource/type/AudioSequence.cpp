#include "AudioSequence.h"

Sequence* AudioSequence::GetPointer() {
    return &sequence;
}

size_t AudioSequence::GetPointerSize() {
    return sizeof(Sequence);
}
