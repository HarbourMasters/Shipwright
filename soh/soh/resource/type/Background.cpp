#include "Background.h"

namespace Ship {
void* Background::GetPointer() {
    return Data.data();
}

size_t Background::GetPointerSize() {
    return Data.size() * sizeof(uint8_t);
}
} // namespace Ship
