#pragma once

#include "SceneCommand.h"
#include "2s2h/resource/type/TextureAnimation.h"

namespace SOH {
typedef struct {
    /* 0x0 */ int8_t segment;
    /* 0x2 */ int16_t type;
    /* 0x4 */ void* params;
} AnimatedMaterialData; // size = 0x8

class SetAnimatedMaterialList : public SceneCommand<AnimatedMaterial> {
  public:
    using SceneCommand::SceneCommand;

    AnimatedMaterial* GetPointer();
    size_t GetPointerSize();

    AnimatedMaterial* mat;
};
}; // namespace SOH
