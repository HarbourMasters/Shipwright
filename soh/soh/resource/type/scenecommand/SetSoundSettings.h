#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
typedef struct {
  uint8_t seqId;
  uint8_t natureAmbienceId;
  uint8_t reverb;
} SoundSettings;

class SetSoundSettings : public SceneCommand<SoundSettings> {
  public:
    using SceneCommand::SceneCommand;

    SoundSettings* GetPointer();
    size_t GetPointerSize();

    SoundSettings settings;
};
}; // namespace LUS
