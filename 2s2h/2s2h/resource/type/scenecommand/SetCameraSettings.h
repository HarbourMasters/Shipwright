#pragma once

#include <cstdint>
#include "SceneCommand.h"

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
}; // namespace SOH
