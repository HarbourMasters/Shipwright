#include "AudioSequence.h"

namespace Ship {

void* AudioSequence::GetPointer() {
    return &sequence;
}

size_t AudioSequence::GetPointerSize() {
    return sizeof(Sequence);
}
} // namespace Ship
