// clang-format off

#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "Resource.h"
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace LUS {
typedef struct {

} CsCamera;

class SetCsCamera : public SceneCommand<CsCamera> {
  public:
    using SceneCommand::SceneCommand;

    CsCamera* GetPointer();
    size_t GetPointerSize();

    CsCamera csCamera;
};
}; // namespace LUS
