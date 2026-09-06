#pragma once

#include "SceneCommand.h"

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
