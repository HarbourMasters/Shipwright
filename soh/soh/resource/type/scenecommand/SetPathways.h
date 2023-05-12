#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
// #include <libultraship/libultra/types.h>
#include "soh/resource/type/Path.h"

namespace LUS {

class SetPathways : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    uint32_t numPaths;
    std::vector<std::shared_ptr<LUS::Path>> paths;
};
}; // namespace LUS
