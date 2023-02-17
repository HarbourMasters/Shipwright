#include "SetPathways.h"

namespace Ship {
void* SetPathways::GetPointer() {
    return paths.data();
}

size_t SetPathways::GetPointerSize() {
    return paths.size() * sizeof(std::shared_ptr<Ship::Path>);
}
} // namespace Ship
