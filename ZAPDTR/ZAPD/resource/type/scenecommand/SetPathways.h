#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>
#include "Path.h"

namespace Ship {

class SetPathways : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    uint32_t numPaths;
    std::vector<std::shared_ptr<Path>> paths;
};
}; // namespace Ship
