#pragma once

#include <cstdint>
#include "SceneCommand.h"

namespace SOH {
typedef struct {
    uint8_t skyboxDisabled;
    uint8_t sunMoonDisabled;
} SkyboxModifier;

class SetSkyboxModifier : public SceneCommand<SkyboxModifier> {
  public:
    using SceneCommand::SceneCommand;

    SkyboxModifier* GetPointer();
    size_t GetPointerSize();

    SkyboxModifier modifier;
};
}; // namespace SOH
