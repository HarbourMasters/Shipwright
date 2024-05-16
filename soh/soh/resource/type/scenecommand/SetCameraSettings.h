#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
typedef struct {
  int8_t cameraMovement;
  int32_t worldMapArea;
} CameraSettings;

class SetCameraSettings : public SceneCommand<CameraSettings> {
  public:
    using SceneCommand::SceneCommand;

    CameraSettings* GetPointer();
    size_t GetPointerSize();

    CameraSettings settings;
};
}; // namespace LUS
