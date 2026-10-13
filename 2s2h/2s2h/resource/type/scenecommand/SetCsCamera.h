#pragma once

#include <vector>
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
typedef struct {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
} z64Vec3s; // size = 0x6

typedef struct {
    /* 0x0 */ s16 setting; // camera setting described by CameraSettingType enum
    /* 0x2 */ s16 count;
    /* 0x4 */ z64Vec3s* actorCsCamFuncData; // s16 data grouped in threes
} ActorCsCamInfoData;                       // size = 0x8

class SetCsCamera : public SceneCommand<ActorCsCamInfoData> {
  public:
    using SceneCommand::SceneCommand;
    ~SetCsCamera();

    ActorCsCamInfoData* GetPointer();
    size_t GetPointerSize();

    std::vector<ActorCsCamInfoData> csCamera;
};
}; // namespace SOH
