#include "SetPathways.h"

namespace SOH {
PathData** SetPathways::GetPointer() {
    return paths.data();
}

size_t SetPathways::GetPointerSize() {
    return paths.size() * sizeof(PathData*);
}
} // namespace SOH
