#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
  int8_t elfMessage;
  int16_t globalObject;
} SpecialObjects;

class SetSpecialObjects : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    SpecialObjects specialObjects;
};
}; // namespace Ship
