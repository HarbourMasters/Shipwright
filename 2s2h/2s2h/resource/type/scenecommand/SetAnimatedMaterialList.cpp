#include "SetAnimatedMaterialList.h"

namespace SOH {

AnimatedMaterial* SetAnimatedMaterialList::GetPointer() {
    return mat;
}

size_t SetAnimatedMaterialList::GetPointerSize() {
    return sizeof(AnimatedMaterial);
}

} // namespace SOH