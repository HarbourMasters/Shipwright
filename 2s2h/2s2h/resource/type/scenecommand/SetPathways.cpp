#include "SetPathways.h"

namespace SOH {
PathDataMM** SetPathwaysMM::GetPointer() {
    return paths.data();
}
size_t SetPathwaysMM::GetPointerSize() {
    return paths.size() * sizeof(PathDataMM*);
}
} // namespace SOH
