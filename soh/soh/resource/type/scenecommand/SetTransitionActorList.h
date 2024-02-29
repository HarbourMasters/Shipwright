#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
// #include <libultraship/libultra/types.h>
#include "z64math.h"

namespace SOH {
typedef struct {
    struct {
        s8 room;    // Room to switch to
        s8 effects; // How the camera reacts during the transition
    } /* 0x00 */ sides[2]; // 0 = front, 1 = back
    /* 0x04 */ s16   id;
    /* 0x06 */ Vec3s pos;
    /* 0x0C */ s16   rotY;
    /* 0x0E */ s16   params;
} TransitionActorEntry; // size = 0x10

class SetTransitionActorList : public SceneCommand<TransitionActorEntry> {
  public:
    using SceneCommand::SceneCommand;

    TransitionActorEntry* GetPointer();
    size_t GetPointerSize();

    uint32_t numTransitionActors;
    std::vector<TransitionActorEntry> transitionActorList;
};
}; // namespace LUS
