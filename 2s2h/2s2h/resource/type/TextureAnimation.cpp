#include "TextureAnimation.h"

namespace SOH {
TextureAnimation::~TextureAnimation() {
    for (auto& a : anims) {
        switch ((TextureAnimationParamsType)a.type) {
            case TextureAnimationParamsType::SingleScroll:
                // a.params is a void* and must be allocated and freed with malloc/free
                free(a.params);
                break;
            case TextureAnimationParamsType::DualScroll:
                free(a.params);
                break;
            case TextureAnimationParamsType::ColorChange:
            case TextureAnimationParamsType::ColorChangeLERP:
            case TextureAnimationParamsType::ColorChangeLagrange:
                delete[]((AnimatedMatColorParams*)a.params)->keyFrames;
                delete[]((AnimatedMatColorParams*)a.params)->primColors;
                delete[]((AnimatedMatColorParams*)a.params)->envColors;
                free(a.params);
                break;
            case TextureAnimationParamsType::TextureCycle:
                // BENTODO free the textures
                delete[]((AnimatedMatTexCycleParams*)a.params)->textureIndexList;
                delete[]((AnimatedMatTexCycleParams*)a.params)->textureList;
                free(a.params);
                break;
        }
    }
}
AnimatedMaterial* TextureAnimation::GetPointer() {
    return anims.data();
}
size_t TextureAnimation::GetPointerSize() {
    return anims.size() * sizeof(AnimatedMaterial);
}
} // namespace SOH