#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
  int8_t hour;
  int8_t minute;
  int8_t timeIncrement;
} TimeSettings;

class SetTimeSettings : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    TimeSettings settings;
};
}; // namespace Ship
