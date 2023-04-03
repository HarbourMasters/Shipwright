#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "soh/resource/type/Cutscene.h"
// #include <libultraship/libultra.h>

namespace Ship {
class SetCutscenes : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    std::string fileName;
    std::shared_ptr<Cutscene> cutscene;
};
}; // namespace Ship
