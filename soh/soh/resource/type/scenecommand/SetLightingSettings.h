#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
    /* 0x00 */ u8 ambientColor[3];
    /* 0x03 */ s8 light1Dir[3];
    /* 0x06 */ u8 light1Color[3];
    /* 0x09 */ s8 light2Dir[3];
    /* 0x0C */ u8 light2Color[3];
    /* 0x0F */ u8 fogColor[3];
    /* 0x12 */ s16 fogNear;
    /* 0x14 */ s16 fogFar;
} EnvLightSettings; // size = 0x16

class SetLightingSettings : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    std::vector<EnvLightSettings> settings;
};
}; // namespace Ship
