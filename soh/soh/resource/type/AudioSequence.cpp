#include "AudioSequence.h"

namespace LUS {

void* AudioSequence::GetPointer() {
    return &sequence;
}

size_t AudioSequence::GetPointerSize() {
    return sizeof(Sequence);
}
} // namespace LUS
