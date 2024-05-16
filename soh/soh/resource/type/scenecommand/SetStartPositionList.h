#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include "soh/resource/type/scenecommand/SetActorList.h"
// #include <libultraship/libultra/types.h>

namespace SOH {
// typedef struct {
//     /* 0x00 */ s16   id;
//     /* 0x02 */ Vec3s pos;
//     /* 0x08 */ Vec3s rot;
//     /* 0x0E */ s16   params;
// } ActorEntry; // size = 0x10

class SetStartPositionList : public SceneCommand<ActorEntry> {
  public:
    using SceneCommand::SceneCommand;

    ActorEntry* GetPointer();
    size_t GetPointerSize();

    uint32_t numStartPositions;
    std::vector<ActorEntry> startPositions;
};
}; // namespace LUS
