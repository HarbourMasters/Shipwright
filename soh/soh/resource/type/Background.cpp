#include "Background.h"

namespace LUS {
void* Background::GetPointer() {
    return Data.data();
}

size_t Background::GetPointerSize() {
    return Data.size() * sizeof(uint8_t);
}
} // namespace LUS
