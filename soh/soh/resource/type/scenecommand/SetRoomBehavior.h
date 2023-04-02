#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
  int8_t gameplayFlags;
  int32_t gameplayFlags2;
} RoomBehavior;

class SetRoomBehavior : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    RoomBehavior roomBehavior;
};
}; // namespace Ship
