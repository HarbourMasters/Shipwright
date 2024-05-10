#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
typedef struct {
  int8_t echo;
} EchoSettings;

class SetEchoSettings : public SceneCommand<EchoSettings> {
  public:
    using SceneCommand::SceneCommand;

    EchoSettings* GetPointer();
    size_t GetPointerSize();

    EchoSettings settings;
};
}; // namespace LUS
