#pragma once

#include <cstdint>
#include "SceneCommand.h"

namespace SOH {
typedef struct {
    int8_t elfMessage;
    int16_t globalObject;
} SpecialObjects;

class SetSpecialObjects : public SceneCommand<SpecialObjects> {
  public:
    using SceneCommand::SceneCommand;

    SpecialObjects* GetPointer();
    size_t GetPointerSize();

    SpecialObjects specialObjects;
};
}; // namespace SOH
