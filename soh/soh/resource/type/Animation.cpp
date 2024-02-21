#include "Animation.h"

namespace SOH {
AnimationData* Animation::GetPointer() {
    return &animationData;
}

size_t Animation::GetPointerSize() {
    switch(type) {
        case AnimationType::Normal:
            return sizeof(animationData.animationHeader);
        case AnimationType::Link:
            return  sizeof(animationData.linkAnimationHeader);
        case AnimationType::Curve:
            return sizeof(animationData.transformUpdateIndex);
        case AnimationType::Legacy:
        default:
            return 0;
    }
}
} // namespace SOH