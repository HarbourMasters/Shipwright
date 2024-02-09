#include "Background.h"

uint8_t* Background::GetPointer() {
    return Data.data();
}

size_t Background::GetPointerSize() {
    return Data.size() * sizeof(uint8_t);
}
