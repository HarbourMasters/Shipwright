#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {
  uint8_t cameraMovement;
  uint32_t worldMapArea;
} CameraSettings;

class SetCameraSettings : public SceneCommand {
  public:
    void* GetPointer();
    size_t GetPointerSize();

    CameraSettings settings;
};
}; // namespace Ship
