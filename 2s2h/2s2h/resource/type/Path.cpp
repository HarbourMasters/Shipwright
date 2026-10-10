#include "Path.h"

namespace SOH {
PathDataMM* PathMM::GetPointer() {
    return pathData.data();
}
size_t PathMM::GetPointerSize() {
    return pathData.size() * sizeof(PathDataMM);
}

} // namespace SOH
