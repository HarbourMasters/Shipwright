#include "SkeletonLimb.h"

namespace SOH {
SkeletonLimbData* SkeletonLimb::GetPointer() {
    return &limbData;
}

size_t SkeletonLimb::GetPointerSize() {
    switch(limbType) {
        case LimbType::Standard:
            return sizeof(limbData.standardLimb);
        case LimbType::LOD:
            return sizeof(limbData.lodLimb);
        case LimbType::Skin:
            return sizeof(limbData.skinLimb);
        case LimbType::Curve:
            return sizeof(limbData.skelCurveLimb);
        case LimbType::Invalid:
        case LimbType::Legacy:
        default:
            return 0;
    }
}
} // namespace SOH
