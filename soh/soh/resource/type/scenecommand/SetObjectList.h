#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
class SetObjectList : public SceneCommand<int16_t> {
  public:
    using SceneCommand::SceneCommand;

    int16_t* GetPointer();
    size_t GetPointerSize();

    uint32_t numObjects;
    std::vector<int16_t> objects;
};
}; // namespace LUS
