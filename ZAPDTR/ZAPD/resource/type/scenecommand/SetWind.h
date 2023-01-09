#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
  int8_t windWest;
  int8_t windVertical;
  int8_t windSouth;
  int8_t windSpeed;
} WindSettings;

class SetWind : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    WindSettings settings;
};
}; // namespace Ship
