#pragma once

#include <cstdint>
#include <vector>
#include <ship/resource/Resource.h>
#include "2s2h/resource/type/scenecommand/SceneCommand.h"
#include "2s2h/resource/type/Cutscene.h"

namespace SOH {
class CutsceneScriptEntry {
  public:
    void* data;
    uint16_t exit;
    uint8_t entrance;
    uint8_t flag;
};

class SetCutscenesMM : public SceneCommand<CutsceneScriptEntry> {
  public:
    using SceneCommand::SceneCommand;

    CutsceneScriptEntry* GetPointer();
    size_t GetPointerSize();

    std::vector<CutsceneScriptEntry> entries;
};

}; // namespace SOH
