#include "AudioSequence.h"

namespace LUS {

Sequence* AudioSequence::GetPointer() {
    return &sequence;
}

size_t AudioSequence::GetPointerSize() {
    return sizeof(Sequence);
}
} // namespace LUS
