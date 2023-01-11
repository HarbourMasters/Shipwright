#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include "Resource.h"
#include "SceneCommand.h"
#include "resource/type/Cutscene.h"
#include <libultraship/libultra/types.h>

namespace Ship {
class SetCutscenes : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    std::string fileName;
    std::shared_ptr<Cutscene> cutscene;
};
}; // namespace Ship
