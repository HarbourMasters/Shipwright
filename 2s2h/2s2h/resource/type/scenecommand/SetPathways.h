#pragma once

#include <cstdint>
#include <vector>
#include "SceneCommand.h"
#include "2s2h/resource/type/Path.h"

namespace SOH {
class SetPathwaysMM : public SceneCommand<PathDataMM*> {
  public:
    using SceneCommand::SceneCommand;

    PathDataMM** GetPointer();
    size_t GetPointerSize();

    uint32_t numPaths;
    std::vector<PathDataMM*> paths;
};

}; // namespace SOH
