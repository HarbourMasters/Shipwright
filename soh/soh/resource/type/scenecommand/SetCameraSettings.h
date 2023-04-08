#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
  int8_t cameraMovement;
  int32_t worldMapArea;
} CameraSettings;

class SetCameraSettings : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    CameraSettings settings;
};
}; // namespace Ship
