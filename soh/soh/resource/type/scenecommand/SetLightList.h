#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
    /* 0x6 */ u8 color[3];
    /* 0x9 */ u8 drawGlow;
    /* 0xA */ s16 radius;
} LightPoint; // size = 0xC

typedef struct {
    /* 0x0 */ s8 x;
    /* 0x1 */ s8 y;
    /* 0x2 */ s8 z;
    /* 0x3 */ u8 color[3];
} LightDirectional; // size = 0x6

typedef union {
    LightPoint point;
    LightDirectional dir;
} LightParams; // size = 0xC

typedef struct {
    /* 0x0 */ u8 type;
    /* 0x2 */ LightParams params;
} LightInfo; // size = 0xE

class SetLightList : public SceneCommand<LightInfo> {
  public:
    using SceneCommand::SceneCommand;

    LightInfo* GetPointer();
    size_t GetPointerSize();

    uint32_t numLights;
    std::vector<LightInfo> lightList;
};
}; // namespace LUS
