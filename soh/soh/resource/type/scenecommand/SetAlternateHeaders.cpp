#include "SetAlternateHeaders.h"

namespace LUS {
void* SetAlternateHeaders::GetPointer() {
    // Like Scene, SetAlternateHeader is a special type that is only acted upon in C++.
    return nullptr;
}

size_t SetAlternateHeaders::GetPointerSize() {
    return 0;
}
} // namespace LUS
