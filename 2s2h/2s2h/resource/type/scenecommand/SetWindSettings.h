#pragma once

#include <cstdint>
#include "SceneCommand.h"

namespace SOH {
typedef struct {
    int8_t windWest;
    int8_t windVertical;
    int8_t windSouth;
    uint8_t windSpeed;
} WindSettings;

class SetWindSettings : public SceneCommand<WindSettings> {
  public:
    using SceneCommand::SceneCommand;

    WindSettings* GetPointer();
    size_t GetPointerSize();

    WindSettings settings;
};
}; // namespace SOH
