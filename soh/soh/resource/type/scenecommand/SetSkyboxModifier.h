#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
  uint8_t skyboxDisabled;
  uint8_t sunMoonDisabled;
} SkyboxModifier;

class SetSkyboxModifier : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    SkyboxModifier modifier;
};
}; // namespace Ship
