#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
class SetExitList : public SceneCommand<uint16_t> {
  public:
    using SceneCommand::SceneCommand;

    uint16_t* GetPointer();
    size_t GetPointerSize();

    uint32_t numExits;

    std::vector<uint16_t> exits;
};
}; // namespace LUS
