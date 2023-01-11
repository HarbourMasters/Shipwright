#include "SetAlternateHeader.h"

namespace Ship {
void* SetAlternateHeader::GetPointer() {
    // Like Scene, SetAlternateHeader is a special type that is only acted upon in C++.
    return nullptr;
}

size_t SetAlternateHeader::GetPointerSize() {
    return 0;
}
} // namespace Ship
