#include "SetPathways.h"

namespace LUS {
void* SetPathways::GetPointer() {
    return paths.data();
}

size_t SetPathways::GetPointerSize() {
    return paths.size() * sizeof(std::shared_ptr<LUS::Path>);
}
} // namespace LUS
