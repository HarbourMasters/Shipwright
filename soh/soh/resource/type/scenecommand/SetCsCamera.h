#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace Ship {
typedef struct {

} CsCamera;

class SetCsCamera : public SceneCommand {
  public:
    using SceneCommand::SceneCommand;

    void* GetPointer();
    size_t GetPointerSize();

    CsCamera csCamera;
};
}; // namespace Ship
