#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
typedef struct {
  uint8_t hour;
  uint8_t minute;
  uint8_t timeIncrement;
} TimeSettings;

class SetTimeSettings : public SceneCommand<TimeSettings> {
  public:
    using SceneCommand::SceneCommand;

    TimeSettings* GetPointer();
    size_t GetPointerSize();

    TimeSettings settings;
};
}; // namespace LUS
