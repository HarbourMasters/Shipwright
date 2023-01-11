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
} SkyboxSettings;

class SetSkyboxSettings : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    SkyboxSettings settings;
};
}; // namespace Ship
