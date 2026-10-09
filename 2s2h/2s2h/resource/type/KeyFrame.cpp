#include "KeyFrame.h"

namespace SOH {

KeyFrameSkel::~KeyFrameSkel() {
    delete[] skelData.limbsFlex;
}

KeyFrameSkeletonData* KeyFrameSkel::GetPointer() {
    return &skelData;
}

size_t KeyFrameSkel::GetPointerSize() {
    return sizeof(skelData);
}

KeyFrameAnim::~KeyFrameAnim() {
    delete[] animData.bitFlags;
    delete[] animData.keyFrames;
    delete[] animData.kfNums;
    delete[] animData.presetValues;
}

KeyFrameAnimationData* KeyFrameAnim::GetPointer() {
    return &animData;
}

size_t KeyFrameAnim::GetPointerSize() {
    return sizeof(animData);
}

} // namespace SOH