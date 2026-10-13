#pragma once

#include <cstdint>
#include "SceneCommand.h"

namespace SOH {
typedef struct {
    int8_t gameplayFlags;
    int8_t currRoomUnk2;
    int8_t currRoomUnk5;
    int8_t msgCtxUnk;
    int8_t enablePointLights;
    int8_t kankyoContextUnkE2;
} RoomBehaviorMM;

class SetRoomBehaviorMM : public SceneCommand<RoomBehaviorMM> {
  public:
    using SceneCommand::SceneCommand;

    RoomBehaviorMM* GetPointer();
    size_t GetPointerSize();

    RoomBehaviorMM roomBehavior;
};

}; // namespace SOH
