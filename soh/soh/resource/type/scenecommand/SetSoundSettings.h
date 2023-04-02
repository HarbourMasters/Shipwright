#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
  uint8_t seqId;
  uint8_t natureAmbienceId;
  uint8_t reverb;
} SoundSettings;

class SetSoundSettings : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    SoundSettings settings;
};
}; // namespace Ship
