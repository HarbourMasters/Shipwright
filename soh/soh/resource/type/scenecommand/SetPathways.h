#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
// #include <libultraship/libultra/types.h>
#include "soh/resource/type/Path.h"

namespace SOH {

class SetPathways : public SceneCommand<PathData*> {
  public:
    using SceneCommand::SceneCommand;

    PathData** GetPointer();
    size_t GetPointerSize();

    uint32_t numPaths;
    std::vector<std::string> pathFileNames;
    std::vector<PathData*> paths;
};
}; // namespace LUS
