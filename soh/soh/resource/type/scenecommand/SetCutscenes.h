#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "soh/resource/type/scenecommand/SceneCommand.h"
#include "soh/resource/type/Cutscene.h"
// #include <libultraship/libultra.h>

namespace SOH {
class SetCutscenes : public SceneCommand<uint32_t> {
  public:
    using SceneCommand::SceneCommand;

    uint32_t* GetPointer();
    size_t GetPointerSize();

    std::string fileName;
    std::shared_ptr<Cutscene> cutscene;
};
}; // namespace LUS
