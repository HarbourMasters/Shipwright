#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
class SetObjectList : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    uint32_t numObjects;
    std::vector<int16_t> objects;
};
}; // namespace Ship
