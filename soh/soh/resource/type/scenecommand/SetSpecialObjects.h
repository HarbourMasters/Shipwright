#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

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
}; // namespace LUS
