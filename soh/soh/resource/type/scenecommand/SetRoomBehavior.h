#pragma once

#include <stdint.h>
#include <memory>
#include "SceneCommand.h"

namespace SOH {
typedef struct {
    int8_t gameplayFlags;
    int32_t gameplayFlags2;
} RoomBehavior;

class SetRoomBehavior : public SceneCommand<RoomBehavior> {
  public:
    using SceneCommand::SceneCommand;

    RoomBehavior* GetPointer();
    size_t GetPointerSize();

    RoomBehavior roomBehavior;
};
}; // namespace SOH
