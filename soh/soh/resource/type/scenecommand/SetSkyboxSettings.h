#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
  uint8_t unk;
  uint8_t skyboxId;
  uint8_t weather;
  uint8_t indoors;
} SkyboxSettings;

class SetSkyboxSettings : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    SkyboxSettings settings;
};
}; // namespace Ship
