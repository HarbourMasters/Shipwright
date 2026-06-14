#pragma once

#include <stdint.h>
#include <vector>
#include <memory>
#include <ship/resource/Resource.h>
#include "SceneCommand.h"
#include <libultraship/libultra/types.h>

namespace SOH {
typedef struct {

} CsCamera;

class SetCsCamera : public SceneCommand<CsCamera> {
  public:
    using SceneCommand::SceneCommand;

    CsCamera* GetPointer();
    size_t GetPointerSize();

    CsCamera csCamera;
};
}; // namespace SOH
