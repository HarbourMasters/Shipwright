#include "SkinAnimatedLimbData.h"

namespace SOH {

SkinVertex* SkinVert::GetPointer() {
    return skinVertexList.data();
}

size_t SkinVert::GetPointerSize() {
    return skinVertexList.size() * sizeof(SkinVertex);
}

SkinTransformation* SkinTransform::GetPointer() {
    return skinTransformationList.data();
}

size_t SkinTransform::GetPointerSize() {
    return skinTransformationList.size() * sizeof(SkinTransformation);
}

SkinLimbModif* SkinModif::GetPointer() {
    return skinLimbModifArray.data();
}

size_t SkinModif::GetPointerSize() {
    return skinLimbModifArray.size() * sizeof(SkinLimbModif);
}

SkinAnimatedLimbData* SkinAnimData::GetPointer() {
    return &data;
}
size_t SkinAnimData::GetPointerSize() {
    return sizeof(SkinAnimatedLimbData);
}
} // namespace SOH