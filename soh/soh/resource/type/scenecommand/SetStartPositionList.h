#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include "soh/resource/type/scenecommand/SetActorList.h"
// #include <libultraship/libultra/types.h>

class SetStartPositionList : public SceneCommand<ActorEntry> {
  public:
    using SceneCommand::SceneCommand;

    ActorEntry* GetPointer();
    size_t GetPointerSize();

    uint32_t numStartPositions;
    std::vector<ActorEntry> startPositions;
};
